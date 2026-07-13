#include "metal_renderer.h"

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/CAMetalLayer.h>
#import <AppKit/AppKit.h>
#include <simd/simd.h>

#include <cstring>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "anchorpoint/core/log.h"

namespace ap {

namespace {

// Uploaded to the vertex shader; column-major simd matrices that, under
// Metal's M * v convention, reproduce the row-vector transforms ap::Matrix4
// uses (so the math layer is shared with the D3D11 backend unchanged).
struct Uniforms {
    simd_float4x4 worldViewProj;
    simd_float4x4 world;
    simd_float4x4 lightViewProj;
};

simd_float4x4 ToSimd(const Matrix4& m) {
    // simd matrices are built from columns; using ap rows as simd columns
    // yields the transpose, which is exactly what M * v_col needs.
    return simd_matrix(
        (simd_float4){m.m[0][0], m.m[0][1], m.m[0][2], m.m[0][3]},
        (simd_float4){m.m[1][0], m.m[1][1], m.m[1][2], m.m[1][3]},
        (simd_float4){m.m[2][0], m.m[2][1], m.m[2][2], m.m[2][3]},
        (simd_float4){m.m[3][0], m.m[3][1], m.m[3][2], m.m[3][3]});
}

MTLPrimitiveType ToMetalTopology(PrimitiveTopology topology) {
    switch (topology) {
        case PrimitiveTopology::LineList:  return MTLPrimitiveTypeLine;
        case PrimitiveTopology::PointList: return MTLPrimitiveTypePoint;
        default:                           return MTLPrimitiveTypeTriangle;
    }
}

} // namespace

struct MetalRenderer::Impl {
    struct MeshResource {
        id<MTLBuffer> vertexBuffer = nil;
        id<MTLTexture> texture = nil;
        // nil when the material has no normal map; the flat default is
        // bound instead at draw time.
        id<MTLTexture> normalMap = nil;
        NSUInteger vertexCount = 0;
        MTLPrimitiveType topology = MTLPrimitiveTypeTriangle;
        simd_float4 diffuseColor = {1, 1, 1, 1};
    };

    id<MTLDevice> device = nil;
    id<MTLCommandQueue> commandQueue = nil;
    CAMetalLayer* layer = nil;
    id<MTLRenderPipelineState> pipeline = nil;
    id<MTLRenderPipelineState> shadowPipeline = nil;
    id<MTLRenderPipelineState> debugPipeline = nil;
    id<MTLDepthStencilState> depthState = nil;
    id<MTLTexture> depthTexture = nil;
    id<MTLSamplerState> sampler = nil;
    id<MTLSamplerState> shadowSampler = nil;
    id<MTLTexture> whiteTexture = nil;
    id<MTLTexture> flatNormalTexture = nil;
    id<MTLTexture> shadowTexture = nil;
    id<MTLBuffer> lightBuffer = nil;
    NSUInteger lightCapacity = 0;
    id<MTLBuffer> debugVertexBuffer = nil;
    NSUInteger debugVertexCapacity = 0;

    std::unordered_map<MeshHandle, MeshResource> meshes;
    MeshHandle nextHandle = 1;
    int width = 0;
    int height = 0;

    bool BuildPipeline(id<MTLLibrary> library);
    bool BuildShadowPipeline(id<MTLLibrary> library);
    bool BuildDebugPipeline(id<MTLLibrary> library);
    bool CreateShadowResources(int size);
    void EnsureDepthTexture(NSUInteger w, NSUInteger h);
    void RenderShadowPass(const FrameData& frame, const Matrix4& lightViewProj,
                          id<MTLCommandBuffer> commandBuffer);
    MTLVertexDescriptor* MakeMeshVertexDescriptor();
    id<MTLTexture> MakeColorTexture(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
};

namespace {

std::string ReadFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) return {};
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

} // namespace

// Vertex layout mirrors ap::Vertex: pos(float3), normal(float3),
// tangent(float3), uv(float2).
MTLVertexDescriptor* MetalRenderer::Impl::MakeMeshVertexDescriptor() {
    MTLVertexDescriptor* vertexDesc = [[MTLVertexDescriptor alloc] init];
    vertexDesc.attributes[0].format = MTLVertexFormatFloat3;
    vertexDesc.attributes[0].offset = 0;
    vertexDesc.attributes[0].bufferIndex = 0;
    vertexDesc.attributes[1].format = MTLVertexFormatFloat3;
    vertexDesc.attributes[1].offset = sizeof(float) * 3;
    vertexDesc.attributes[1].bufferIndex = 0;
    vertexDesc.attributes[2].format = MTLVertexFormatFloat3;
    vertexDesc.attributes[2].offset = sizeof(float) * 6;
    vertexDesc.attributes[2].bufferIndex = 0;
    vertexDesc.attributes[3].format = MTLVertexFormatFloat2;
    vertexDesc.attributes[3].offset = sizeof(float) * 9;
    vertexDesc.attributes[3].bufferIndex = 0;
    vertexDesc.layouts[0].stride = sizeof(float) * 11;
    return vertexDesc;
}

bool MetalRenderer::Impl::BuildPipeline(id<MTLLibrary> library) {
    NSError* error = nil;
    id<MTLFunction> vertexFn = [library newFunctionWithName:@"vertex_main"];
    id<MTLFunction> fragmentFn = [library newFunctionWithName:@"fragment_main"];

    MTLRenderPipelineDescriptor* desc = [[MTLRenderPipelineDescriptor alloc] init];
    desc.vertexFunction = vertexFn;
    desc.fragmentFunction = fragmentFn;
    desc.vertexDescriptor = MakeMeshVertexDescriptor();
    desc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    desc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;

    pipeline = [device newRenderPipelineStateWithDescriptor:desc error:&error];
    if (pipeline == nil) {
        LogError(error ? error.localizedDescription.UTF8String : "Failed to create pipeline state");
        return false;
    }

    MTLDepthStencilDescriptor* depthDesc = [[MTLDepthStencilDescriptor alloc] init];
    depthDesc.depthCompareFunction = MTLCompareFunctionLess;
    depthDesc.depthWriteEnabled = YES;
    depthState = [device newDepthStencilStateWithDescriptor:depthDesc];

    MTLSamplerDescriptor* sampDesc = [[MTLSamplerDescriptor alloc] init];
    sampDesc.minFilter = MTLSamplerMinMagFilterLinear;
    sampDesc.magFilter = MTLSamplerMinMagFilterLinear;
    sampDesc.mipFilter = MTLSamplerMipFilterLinear;
    sampDesc.sAddressMode = MTLSamplerAddressModeRepeat;
    sampDesc.tAddressMode = MTLSamplerAddressModeRepeat;
    sampler = [device newSamplerStateWithDescriptor:sampDesc];

    return true;
}

// Depth-only pipeline for rendering the shadow map. Reuses the main vertex
// shader with no fragment function and no colour attachment.
bool MetalRenderer::Impl::BuildShadowPipeline(id<MTLLibrary> library) {
    id<MTLFunction> vertexFn = [library newFunctionWithName:@"vertex_main"];
    if (vertexFn == nil)
        return false;

    MTLRenderPipelineDescriptor* desc = [[MTLRenderPipelineDescriptor alloc] init];
    desc.vertexFunction = vertexFn;
    desc.fragmentFunction = nil;
    desc.vertexDescriptor = MakeMeshVertexDescriptor();
    desc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;

    NSError* error = nil;
    shadowPipeline = [device newRenderPipelineStateWithDescriptor:desc error:&error];
    if (shadowPipeline == nil) {
        LogError(error ? error.localizedDescription.UTF8String : "Failed to create shadow pipeline");
        return false;
    }
    return true;
}

bool MetalRenderer::Impl::CreateShadowResources(int size) {
    MTLTextureDescriptor* desc =
        [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                                                           width:size
                                                          height:size
                                                       mipmapped:NO];
    desc.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
    desc.storageMode = MTLStorageModePrivate;
    shadowTexture = [device newTextureWithDescriptor:desc];

    MTLSamplerDescriptor* sampDesc = [[MTLSamplerDescriptor alloc] init];
    sampDesc.minFilter = MTLSamplerMinMagFilterLinear;
    sampDesc.magFilter = MTLSamplerMinMagFilterLinear;
    sampDesc.sAddressMode = MTLSamplerAddressModeClampToEdge;
    sampDesc.tAddressMode = MTLSamplerAddressModeClampToEdge;
    sampDesc.compareFunction = MTLCompareFunctionLessEqual;
    shadowSampler = [device newSamplerStateWithDescriptor:sampDesc];

    return shadowTexture != nil && shadowSampler != nil;
}

bool MetalRenderer::Impl::BuildDebugPipeline(id<MTLLibrary> library) {
    id<MTLFunction> vertexFn = [library newFunctionWithName:@"debug_vertex_main"];
    id<MTLFunction> fragmentFn = [library newFunctionWithName:@"debug_fragment_main"];
    if (vertexFn == nil || fragmentFn == nil)
        return false;

    // Debug vertex layout mirrors ap::DebugVertex: pos(float3), color(float3).
    MTLVertexDescriptor* vertexDesc = [[MTLVertexDescriptor alloc] init];
    vertexDesc.attributes[0].format = MTLVertexFormatFloat3;
    vertexDesc.attributes[0].offset = 0;
    vertexDesc.attributes[0].bufferIndex = 0;
    vertexDesc.attributes[1].format = MTLVertexFormatFloat3;
    vertexDesc.attributes[1].offset = sizeof(float) * 3;
    vertexDesc.attributes[1].bufferIndex = 0;
    vertexDesc.layouts[0].stride = sizeof(float) * 6;

    MTLRenderPipelineDescriptor* desc = [[MTLRenderPipelineDescriptor alloc] init];
    desc.vertexFunction = vertexFn;
    desc.fragmentFunction = fragmentFn;
    desc.vertexDescriptor = vertexDesc;
    desc.inputPrimitiveTopology = MTLPrimitiveTopologyClassLine;
    desc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    desc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;

    NSError* error = nil;
    debugPipeline = [device newRenderPipelineStateWithDescriptor:desc error:&error];
    if (debugPipeline == nil) {
        LogError(error ? error.localizedDescription.UTF8String : "Failed to create debug pipeline");
        return false;
    }
    return true;
}

void MetalRenderer::Impl::EnsureDepthTexture(NSUInteger w, NSUInteger h) {
    if (depthTexture != nil && depthTexture.width == w && depthTexture.height == h)
        return;

    MTLTextureDescriptor* desc =
        [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                                                           width:w
                                                          height:h
                                                       mipmapped:NO];
    desc.usage = MTLTextureUsageRenderTarget;
    desc.storageMode = MTLStorageModePrivate;
    depthTexture = [device newTextureWithDescriptor:desc];
}

id<MTLTexture> MetalRenderer::Impl::MakeColorTexture(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    MTLTextureDescriptor* desc =
        [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                           width:1
                                                          height:1
                                                       mipmapped:NO];
    id<MTLTexture> texture = [device newTextureWithDescriptor:desc];
    const uint8_t pixel[4] = {r, g, b, a};
    [texture replaceRegion:MTLRegionMake2D(0, 0, 1, 1)
               mipmapLevel:0
                 withBytes:pixel
               bytesPerRow:4];
    return texture;
}

MetalRenderer::MetalRenderer() : impl(std::make_unique<Impl>()) {}
MetalRenderer::~MetalRenderer() = default;

bool MetalRenderer::Init(Window& window, const EngineConfig& config) {
    @autoreleasepool {
        NSView* view = (__bridge NSView*)window.GetNativeHandle();
        if (view == nil) {
            LogError("MetalRenderer requires a native Cocoa view");
            return false;
        }

        impl->device = MTLCreateSystemDefaultDevice();
        if (impl->device == nil) {
            LogError("No Metal-capable device found");
            return false;
        }

        impl->commandQueue = [impl->device newCommandQueue];
        impl->width = config.width;
        impl->height = config.height;

        impl->layer = [CAMetalLayer layer];
        impl->layer.device = impl->device;
        impl->layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        impl->layer.framebufferOnly = YES;
        const CGFloat scale = view.window ? view.window.backingScaleFactor : 1.0;
        impl->layer.contentsScale = scale;
        impl->layer.drawableSize = CGSizeMake(config.width * scale, config.height * scale);
        view.layer = impl->layer;
        view.wantsLayer = YES;

        const std::string source = ReadFile("shaders/Shader.metal");
        if (source.empty()) {
            LogError("Failed to read shaders/Shader.metal");
            return false;
        }
        NSError* libError = nil;
        id<MTLLibrary> library = [impl->device newLibraryWithSource:@(source.c_str())
                                                            options:nil
                                                              error:&libError];
        if (library == nil) {
            LogError(libError ? libError.localizedDescription.UTF8String : "Failed to compile Metal library");
            return false;
        }

        if (!impl->BuildPipeline(library))
            return false;
        if (!impl->BuildShadowPipeline(library) || !impl->CreateShadowResources(config.shadowMapSize))
            LogError("Shadows unavailable (failed to create shadow pipeline)");
        if (!impl->BuildDebugPipeline(library))
            LogError("Debug collider overlay unavailable (failed to build debug pipeline)");

        impl->whiteTexture = impl->MakeColorTexture(255, 255, 255, 255);
        // (128, 128, 255) decodes to the (0, 0, 1) tangent-space normal: flat.
        impl->flatNormalTexture = impl->MakeColorTexture(128, 128, 255, 255);
        impl->EnsureDepthTexture(impl->layer.drawableSize.width,
                                 impl->layer.drawableSize.height);
        return true;
    }
}

MeshHandle MetalRenderer::CreateMesh(const MeshData& data) {
    @autoreleasepool {
        if (data.vertices.empty())
            return kInvalidMesh;

        Impl::MeshResource mesh;
        mesh.vertexCount = data.vertices.size();
        mesh.topology = ToMetalTopology(data.topology);
        mesh.diffuseColor = {data.material.diffuseColor.x,
                             data.material.diffuseColor.y,
                             data.material.diffuseColor.z, 1.0f};

        mesh.vertexBuffer = [impl->device newBufferWithBytes:data.vertices.data()
                                                      length:data.vertices.size() * sizeof(Vertex)
                                                     options:MTLResourceStorageModeShared];

        MTKTextureLoader* loader = [[MTKTextureLoader alloc] initWithDevice:impl->device];
        NSDictionary* options = @{
            MTKTextureLoaderOptionOrigin : MTKTextureLoaderOriginTopLeft,
            MTKTextureLoaderOptionSRGB : @NO,
        };

        if (!data.material.texture.empty()) {
            NSURL* url = [NSURL fileURLWithPath:@(data.material.texture.c_str())];
            NSError* error = nil;
            mesh.texture = [loader newTextureWithContentsOfURL:url options:options error:&error];
            if (mesh.texture == nil)
                LogError(error ? error.localizedDescription.UTF8String : "Failed to load texture");
        }

        if (!data.material.normalMap.empty()) {
            NSURL* url = [NSURL fileURLWithPath:@(data.material.normalMap.c_str())];
            NSError* error = nil;
            mesh.normalMap = [loader newTextureWithContentsOfURL:url options:options error:&error];
            if (mesh.normalMap == nil)
                LogError(error ? error.localizedDescription.UTF8String : "Failed to load normal map");
        }

        const MeshHandle handle = impl->nextHandle++;
        impl->meshes[handle] = mesh;
        return handle;
    }
}

void MetalRenderer::Impl::RenderShadowPass(const FrameData& frame, const Matrix4& lightViewProj,
                                           id<MTLCommandBuffer> commandBuffer) {
    MTLRenderPassDescriptor* pass = [MTLRenderPassDescriptor renderPassDescriptor];
    pass.depthAttachment.texture = shadowTexture;
    pass.depthAttachment.loadAction = MTLLoadActionClear;
    pass.depthAttachment.storeAction = MTLStoreActionStore;
    pass.depthAttachment.clearDepth = 1.0;

    id<MTLRenderCommandEncoder> encoder =
        [commandBuffer renderCommandEncoderWithDescriptor:pass];
    encoder.label = @"AnchorPoint Shadow";
    [encoder setRenderPipelineState:shadowPipeline];
    [encoder setDepthStencilState:depthState];
    [encoder setCullMode:MTLCullModeNone];

    for (const DrawItem& item : frame.items) {
        const auto it = meshes.find(item.mesh);
        if (it == meshes.end()) continue;
        MeshResource& mesh = it->second;

        Uniforms uniforms;
        uniforms.worldViewProj = ToSimd(item.world * lightViewProj);
        uniforms.world = ToSimd(item.world);
        uniforms.lightViewProj = ToSimd(Matrix4::Identity());

        [encoder setVertexBuffer:mesh.vertexBuffer offset:0 atIndex:0];
        [encoder setVertexBytes:&uniforms length:sizeof(uniforms) atIndex:1];
        [encoder drawPrimitives:mesh.topology vertexStart:0 vertexCount:mesh.vertexCount];
    }

    [encoder endEncoding];
}

void MetalRenderer::RenderFrame(const FrameData& frame) {
    @autoreleasepool {
        id<CAMetalDrawable> drawable = [impl->layer nextDrawable];
        if (drawable == nil)
            return;

        const NSUInteger w = impl->layer.drawableSize.width;
        const NSUInteger h = impl->layer.drawableSize.height;
        impl->EnsureDepthTexture(w, h);

        // Grow the shared light buffer to fit this frame's lights.
        if (!frame.lights.empty() && frame.lights.size() > impl->lightCapacity) {
            impl->lightBuffer = [impl->device newBufferWithLength:frame.lights.size() * sizeof(GpuLight)
                                                          options:MTLResourceStorageModeShared];
            impl->lightCapacity = frame.lights.size();
        }
        if (!frame.lights.empty()) {
            std::memcpy(impl->lightBuffer.contents, frame.lights.data(),
                        frame.lights.size() * sizeof(GpuLight));
        }

        id<MTLCommandBuffer> commandBuffer = [impl->commandQueue commandBuffer];

        Matrix4 lightViewProj;
        const bool hasShadows = DirectionalLightViewProj(frame, lightViewProj)
                              && impl->shadowPipeline != nil
                              && impl->shadowTexture != nil;
        if (hasShadows) {
            impl->RenderShadowPass(frame, lightViewProj, commandBuffer);
        } else {
            // All zeros makes lightSpacePos.w == 0, which the fragment shader
            // reads as "no shadows".
            for (auto& row : lightViewProj.m)
                for (float& value : row)
                    value = 0.0f;
        }

        MTLRenderPassDescriptor* pass = [MTLRenderPassDescriptor renderPassDescriptor];
        pass.colorAttachments[0].texture = drawable.texture;
        pass.colorAttachments[0].loadAction = MTLLoadActionClear;
        pass.colorAttachments[0].storeAction = MTLStoreActionStore;
        pass.colorAttachments[0].clearColor = MTLClearColorMake(
            frame.clearColor[0], frame.clearColor[1], frame.clearColor[2], frame.clearColor[3]);
        pass.depthAttachment.texture = impl->depthTexture;
        pass.depthAttachment.loadAction = MTLLoadActionClear;
        pass.depthAttachment.storeAction = MTLStoreActionDontCare;
        pass.depthAttachment.clearDepth = 1.0;

        id<MTLRenderCommandEncoder> encoder =
            [commandBuffer renderCommandEncoderWithDescriptor:pass];

        encoder.label = @"AnchorPoint";
        [encoder setRenderPipelineState:impl->pipeline];
        [encoder setDepthStencilState:impl->depthState];
        // No culling: matches the engine's other backends and avoids
        // winding-order surprises across model sources.
        [encoder setCullMode:MTLCullModeNone];
        [encoder setFragmentSamplerState:impl->sampler atIndex:0];
        if (impl->shadowSampler != nil)
            [encoder setFragmentSamplerState:impl->shadowSampler atIndex:1];
        // The shadow texture must always be bound (the shader declares it);
        // when there are no shadows the zero light matrix skips sampling it.
        if (impl->shadowTexture != nil)
            [encoder setFragmentTexture:impl->shadowTexture atIndex:2];

        const Matrix4 viewProj = frame.view * frame.projection;
        const auto lightCount = static_cast<uint32_t>(frame.lights.size());

        for (const DrawItem& item : frame.items) {
            const auto it = impl->meshes.find(item.mesh);
            if (it == impl->meshes.end()) continue;
            Impl::MeshResource& mesh = it->second;

            Uniforms uniforms;
            uniforms.worldViewProj = ToSimd(item.world * viewProj);
            uniforms.world = ToSimd(item.world);
            uniforms.lightViewProj = ToSimd(lightViewProj);

            [encoder setVertexBuffer:mesh.vertexBuffer offset:0 atIndex:0];
            [encoder setVertexBytes:&uniforms length:sizeof(uniforms) atIndex:1];

            [encoder setFragmentBytes:&mesh.diffuseColor length:sizeof(mesh.diffuseColor) atIndex:0];
            if (impl->lightBuffer != nil)
                [encoder setFragmentBuffer:impl->lightBuffer offset:0 atIndex:1];
            [encoder setFragmentBytes:&lightCount length:sizeof(lightCount) atIndex:2];
            [encoder setFragmentTexture:(mesh.texture ? mesh.texture : impl->whiteTexture) atIndex:0];
            [encoder setFragmentTexture:(mesh.normalMap ? mesh.normalMap : impl->flatNormalTexture)
                                atIndex:1];

            [encoder drawPrimitives:mesh.topology vertexStart:0 vertexCount:mesh.vertexCount];
        }

        if (!frame.debugLines.empty() && impl->debugPipeline != nil) {
            const NSUInteger byteCount = frame.debugLines.size() * sizeof(DebugVertex);
            if (frame.debugLines.size() > impl->debugVertexCapacity) {
                impl->debugVertexBuffer = [impl->device newBufferWithLength:byteCount
                                                                    options:MTLResourceStorageModeShared];
                impl->debugVertexCapacity = frame.debugLines.size();
            }
            std::memcpy(impl->debugVertexBuffer.contents, frame.debugLines.data(), byteCount);

            simd_float4x4 viewProjSimd = ToSimd(viewProj);
            [encoder setRenderPipelineState:impl->debugPipeline];
            [encoder setVertexBuffer:impl->debugVertexBuffer offset:0 atIndex:0];
            [encoder setVertexBytes:&viewProjSimd length:sizeof(viewProjSimd) atIndex:1];
            [encoder drawPrimitives:MTLPrimitiveTypeLine
                        vertexStart:0
                        vertexCount:frame.debugLines.size()];
        }

        [encoder endEncoding];
        [commandBuffer presentDrawable:drawable];
        [commandBuffer commit];
    }
}

void MetalRenderer::Shutdown() {
    // ARC releases the Objective-C objects when Impl is destroyed; clearing the
    // mesh table drops their references now.
    impl->meshes.clear();
    impl->lightBuffer = nil;
    impl->debugVertexBuffer = nil;
    impl->depthTexture = nil;
    impl->shadowTexture = nil;
    impl->whiteTexture = nil;
    impl->flatNormalTexture = nil;
    impl->sampler = nil;
    impl->shadowSampler = nil;
    impl->depthState = nil;
    impl->debugPipeline = nil;
    impl->shadowPipeline = nil;
    impl->pipeline = nil;
    impl->layer = nil;
    impl->commandQueue = nil;
    impl->device = nil;
}

} // namespace ap
