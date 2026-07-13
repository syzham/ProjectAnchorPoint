#include "d3d11_renderer.h"

#include <cstring>
#include <d3dcompiler.h>

#include "WICTextureLoader11.h"
#include "anchorpoint/core/log.h"

namespace ap {

namespace {

// Matches the ObjectData cbuffer in the vertex shaders (row_major matrices,
// uploaded transposed like the original engine did).
struct ObjectBuffer {
    float worldViewProj[4][4];
    float world[4][4];
    float lightViewProj[4][4];
};

struct MaterialBuffer {
    float diffuseColor[4];
};

void StoreTransposed(const Matrix4& matrix, float out[4][4]) {
    const Matrix4 transposed = matrix.Transposed();
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            out[i][j] = transposed.m[i][j];
}

D3D11_PRIMITIVE_TOPOLOGY ToD3DTopology(PrimitiveTopology topology) {
    switch (topology) {
        case PrimitiveTopology::LineList: return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
        case PrimitiveTopology::PointList: return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
        default: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }
}

std::wstring Widen(const std::string& str) {
    return {str.begin(), str.end()};
}

template<typename T>
void SafeRelease(T*& resource) {
    if (resource) {
        resource->Release();
        resource = nullptr;
    }
}

} // namespace

bool D3D11Renderer::Init(Window& window, const EngineConfig& config) {
    HWND hWnd = static_cast<HWND>(window.GetNativeHandle());
    if (!hWnd) {
        LogError("D3D11Renderer requires a native window");
        return false;
    }

    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferDesc.Height = config.height;
    scd.BufferDesc.Width = config.width;
    scd.OutputWindow = hWnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr,
                                               D3D_DRIVER_TYPE_HARDWARE,
                                               nullptr,
                                               0,
                                               nullptr,
                                               0,
                                               D3D11_SDK_VERSION,
                                               &scd,
                                               &swapChain,
                                               &device,
                                               nullptr,
                                               &context);
    if (FAILED(hr)) {
        LogError("D3D11CreateDeviceAndSwapChain failed", hr);
        return false;
    }

    ID3D11Texture2D* pBackBuffer = nullptr;
    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer));
    device->CreateRenderTargetView(pBackBuffer, nullptr, &backBuffer);
    pBackBuffer->Release();

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(config.width);
    viewport.Height = static_cast<float>(config.height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    context->RSSetViewports(1, &viewport);

    D3D11_BUFFER_DESC objDesc = {};
    objDesc.ByteWidth = sizeof(ObjectBuffer);
    objDesc.Usage = D3D11_USAGE_DYNAMIC;
    objDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    objDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    device->CreateBuffer(&objDesc, nullptr, &objectBuffer);

    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = config.width;
    depthDesc.Height = config.height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ID3D11Texture2D* depthTexture = nullptr;
    device->CreateTexture2D(&depthDesc, nullptr, &depthTexture);
    device->CreateDepthStencilView(depthTexture, nullptr, &depthView);
    depthTexture->Release();

    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    device->CreateDepthStencilState(&dsDesc, &depthState);

    screenWidth = config.width;
    screenHeight = config.height;

    if (!LoadDebugShader())
        LogError("Debug collider overlay unavailable (failed to load debug shaders)");
    if (!CreateShadowResources(config.shadowMapSize))
        LogError("Shadows unavailable (failed to create shadow map)");
    if (!CreateDefaultNormalTexture())
        LogError("Failed to create default normal texture");

    return true;
}

bool D3D11Renderer::CreateShadowResources(int size) {
    shadowMapSize = size;

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = size;
    texDesc.Height = size;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

    ID3D11Texture2D* texture = nullptr;
    if (FAILED(device->CreateTexture2D(&texDesc, nullptr, &texture)))
        return false;

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    HRESULT hr = device->CreateDepthStencilView(texture, &dsvDesc, &shadowDSV);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    if (SUCCEEDED(hr))
        hr = device->CreateShaderResourceView(texture, &srvDesc, &shadowSRV);
    texture->Release();
    if (FAILED(hr))
        return false;

    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = sampDesc.AddressV = sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    return SUCCEEDED(device->CreateSamplerState(&sampDesc, &shadowSampler));
}

bool D3D11Renderer::CreateDefaultNormalTexture() {
    // (128, 128, 255) decodes to the (0, 0, 1) tangent-space normal: flat.
    const unsigned char flat[4] = {128, 128, 255, 255};

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = 1;
    texDesc.Height = 1;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_IMMUTABLE;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = flat;
    initData.SysMemPitch = 4;

    ID3D11Texture2D* texture = nullptr;
    if (FAILED(device->CreateTexture2D(&texDesc, &initData, &texture)))
        return false;
    const HRESULT hr = device->CreateShaderResourceView(texture, nullptr, &defaultNormalSRV);
    texture->Release();
    return SUCCEEDED(hr);
}

bool D3D11Renderer::LoadDebugShader() {
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    HRESULT hr = D3DCompileFromFile(L"shaders/DebugVS.hlsl", nullptr, nullptr,
                                    "main", "vs_5_0", 0, 0, &vsBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            LogError(static_cast<const char*>(errorBlob->GetBufferPointer()));
            errorBlob->Release();
        }
        return false;
    }

    hr = D3DCompileFromFile(L"shaders/DebugPS.hlsl", nullptr, nullptr,
                            "main", "ps_5_0", 0, 0, &psBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            LogError(static_cast<const char*>(errorBlob->GetBufferPointer()));
            errorBlob->Release();
        }
        vsBlob->Release();
        return false;
    }

    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &debugShader.vertexShader);
    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &debugShader.pixelShader);

    const D3D11_INPUT_ELEMENT_DESC layout[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    hr = device->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(),
                                   vsBlob->GetBufferSize(), &debugShader.inputLayout);
    vsBlob->Release();
    psBlob->Release();
    return SUCCEEDED(hr);
}

bool D3D11Renderer::LoadShader(const std::wstring& vsPath, const std::wstring& psPath,
                               ShaderProgram& out) {
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    HRESULT hr = D3DCompileFromFile(vsPath.c_str(), nullptr, nullptr, "main", "vs_5_0", 0, 0, &vsBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            LogError(static_cast<const char*>(errorBlob->GetBufferPointer()));
            errorBlob->Release();
        }
        return false;
    }

    hr = D3DCompileFromFile(psPath.c_str(), nullptr, nullptr, "main", "ps_5_0", 0, 0, &psBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            LogError(static_cast<const char*>(errorBlob->GetBufferPointer()));
            errorBlob->Release();
        }
        vsBlob->Release();
        return false;
    }

    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &out.vertexShader);
    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &out.pixelShader);

    const D3D11_INPUT_ELEMENT_DESC layout[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 6, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 9, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    hr = device->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(),
                                   vsBlob->GetBufferSize(), &out.inputLayout);
    vsBlob->Release();
    psBlob->Release();
    return SUCCEEDED(hr);
}

MeshHandle D3D11Renderer::CreateMesh(const MeshData& data) {
    if (data.vertices.empty())
        return kInvalidMesh;

    MeshResource mesh;
    mesh.vertexCount = static_cast<UINT>(data.vertices.size());
    mesh.topology = ToD3DTopology(data.topology);
    mesh.diffuseColor[0] = data.material.diffuseColor.x;
    mesh.diffuseColor[1] = data.material.diffuseColor.y;
    mesh.diffuseColor[2] = data.material.diffuseColor.z;
    mesh.diffuseColor[3] = 1.0f;

    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * data.vertices.size());
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = data.vertices.data();

    if (FAILED(device->CreateBuffer(&desc, &initData, &mesh.vertexBuffer)))
        return kInvalidMesh;

    D3D11_BUFFER_DESC matDesc = {};
    matDesc.ByteWidth = sizeof(MaterialBuffer);
    matDesc.Usage = D3D11_USAGE_DYNAMIC;
    matDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    device->CreateBuffer(&matDesc, nullptr, &mesh.materialBuffer);

    if (!data.material.texture.empty())
        DirectX::CreateWICTextureFromFile(device, Widen(data.material.texture).c_str(),
                                          nullptr, &mesh.textureSRV);

    if (!data.material.normalMap.empty())
        DirectX::CreateWICTextureFromFile(device, Widen(data.material.normalMap).c_str(),
                                          nullptr, &mesh.normalMapSRV);

    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = sampDesc.AddressV = sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    device->CreateSamplerState(&sampDesc, &mesh.samplerState);

    if (!LoadShader(Widen(data.material.vertexShader), Widen(data.material.pixelShader), mesh.shader)) {
        LogError("Failed to load shaders for mesh");
        ReleaseMesh(mesh);
        return kInvalidMesh;
    }

    const MeshHandle handle = nextHandle++;
    meshes[handle] = mesh;
    return handle;
}

void D3D11Renderer::UpdateLightBuffer(const std::vector<GpuLight>& lights) {
    if (lights.empty()) {
        ID3D11ShaderResourceView* nullSRV = nullptr;
        context->PSSetShaderResources(0, 1, &nullSRV);
        return;
    }

    if (lights.size() > lightCapacity) {
        SafeRelease(lightSRV);
        SafeRelease(lightBuffer);

        D3D11_BUFFER_DESC lightDesc = {};
        lightDesc.Usage = D3D11_USAGE_DEFAULT;
        lightDesc.ByteWidth = static_cast<UINT>(sizeof(GpuLight) * lights.size());
        lightDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        lightDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        lightDesc.StructureByteStride = sizeof(GpuLight);

        device->CreateBuffer(&lightDesc, nullptr, &lightBuffer);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        srvDesc.Buffer.NumElements = static_cast<UINT>(lights.size());

        device->CreateShaderResourceView(lightBuffer, &srvDesc, &lightSRV);
        lightCapacity = lights.size();
    }

    context->UpdateSubresource(lightBuffer, 0, nullptr, lights.data(), 0, 0);
    context->PSSetShaderResources(0, 1, &lightSRV);
}

void D3D11Renderer::RenderShadowPass(const FrameData& frame, const Matrix4& lightViewProj) {
    // The shadow map was bound as a PS resource last frame; unbind it before
    // reusing it as the depth target.
    ID3D11ShaderResourceView* nullSRV = nullptr;
    context->PSSetShaderResources(3, 1, &nullSRV);

    context->ClearDepthStencilView(shadowDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
    context->OMSetRenderTargets(0, nullptr, shadowDSV);
    context->OMSetDepthStencilState(depthState, 0);

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(shadowMapSize);
    viewport.Height = static_cast<float>(shadowMapSize);
    viewport.MaxDepth = 1.0f;
    context->RSSetViewports(1, &viewport);

    // Depth-only: reuse each mesh's vertex shader with no pixel shader.
    context->PSSetShader(nullptr, nullptr, 0);

    for (const DrawItem& item : frame.items) {
        const auto it = meshes.find(item.mesh);
        if (it == meshes.end()) continue;
        MeshResource& mesh = it->second;

        D3D11_MAPPED_SUBRESOURCE mapped = {};
        context->Map(objectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        auto* objBuffer = static_cast<ObjectBuffer*>(mapped.pData);
        StoreTransposed(item.world * lightViewProj, objBuffer->worldViewProj);
        StoreTransposed(item.world, objBuffer->world);
        StoreTransposed(Matrix4::Identity(), objBuffer->lightViewProj);
        context->Unmap(objectBuffer, 0);
        context->VSSetConstantBuffers(1, 1, &objectBuffer);

        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, &mesh.vertexBuffer, &stride, &offset);
        context->IASetPrimitiveTopology(mesh.topology);
        context->IASetInputLayout(mesh.shader.inputLayout);
        context->VSSetShader(mesh.shader.vertexShader, nullptr, 0);

        context->Draw(mesh.vertexCount, 0);
    }

    // Restore the screen viewport for the main pass.
    D3D11_VIEWPORT screenViewport = {};
    screenViewport.Width = static_cast<float>(screenWidth);
    screenViewport.Height = static_cast<float>(screenHeight);
    screenViewport.MaxDepth = 1.0f;
    context->RSSetViewports(1, &screenViewport);
}

void D3D11Renderer::RenderFrame(const FrameData& frame) {
    Matrix4 lightViewProj;
    const bool hasShadows = DirectionalLightViewProj(frame, lightViewProj)
                          && shadowDSV != nullptr;
    if (hasShadows) {
        RenderShadowPass(frame, lightViewProj);
    } else {
        // All zeros makes lightSpacePos.w == 0, which the pixel shader reads
        // as "no shadows".
        for (auto& row : lightViewProj.m)
            for (float& value : row)
                value = 0.0f;
    }

    context->ClearRenderTargetView(backBuffer, frame.clearColor);
    context->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &backBuffer, depthView);
    context->OMSetDepthStencilState(depthState, 0);

    UpdateLightBuffer(frame.lights);

    if (hasShadows)
        context->PSSetShaderResources(3, 1, &shadowSRV);
    context->PSSetSamplers(1, 1, &shadowSampler);

    const Matrix4 viewProj = frame.view * frame.projection;

    for (const DrawItem& item : frame.items) {
        const auto it = meshes.find(item.mesh);
        if (it == meshes.end()) continue;
        MeshResource& mesh = it->second;

        D3D11_MAPPED_SUBRESOURCE mapped = {};
        context->Map(objectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        auto* objBuffer = static_cast<ObjectBuffer*>(mapped.pData);
        StoreTransposed(item.world * viewProj, objBuffer->worldViewProj);
        StoreTransposed(item.world, objBuffer->world);
        StoreTransposed(lightViewProj, objBuffer->lightViewProj);
        context->Unmap(objectBuffer, 0);
        context->VSSetConstantBuffers(1, 1, &objectBuffer);

        D3D11_MAPPED_SUBRESOURCE matMapped = {};
        context->Map(mesh.materialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &matMapped);
        auto* materialBuffer = static_cast<MaterialBuffer*>(matMapped.pData);
        for (int i = 0; i < 4; ++i)
            materialBuffer->diffuseColor[i] = mesh.diffuseColor[i];
        context->Unmap(mesh.materialBuffer, 0);
        context->PSSetConstantBuffers(0, 1, &mesh.materialBuffer);

        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, &mesh.vertexBuffer, &stride, &offset);
        context->IASetPrimitiveTopology(mesh.topology);

        context->PSSetShaderResources(1, 1, &mesh.textureSRV);
        ID3D11ShaderResourceView* normalSRV =
            mesh.normalMapSRV ? mesh.normalMapSRV : defaultNormalSRV;
        context->PSSetShaderResources(2, 1, &normalSRV);
        context->PSSetSamplers(0, 1, &mesh.samplerState);

        context->IASetInputLayout(mesh.shader.inputLayout);
        context->VSSetShader(mesh.shader.vertexShader, nullptr, 0);
        context->PSSetShader(mesh.shader.pixelShader, nullptr, 0);

        context->Draw(mesh.vertexCount, 0);
    }

    if (!frame.debugLines.empty())
        DrawDebugLines(frame.debugLines, viewProj);

    swapChain->Present(0, 0);
}

void D3D11Renderer::DrawDebugLines(const std::vector<DebugVertex>& lines, const Matrix4& viewProj) {
    if (debugShader.vertexShader == nullptr) return;

    // Grow the dynamic vertex buffer to fit this frame's lines.
    if (lines.size() > debugVertexCapacity) {
        SafeRelease(debugVertexBuffer);
        D3D11_BUFFER_DESC desc = {};
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = static_cast<UINT>(sizeof(DebugVertex) * lines.size());
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        device->CreateBuffer(&desc, nullptr, &debugVertexBuffer);
        debugVertexCapacity = lines.size();
    }

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    context->Map(debugVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    std::memcpy(mapped.pData, lines.data(), sizeof(DebugVertex) * lines.size());
    context->Unmap(debugVertexBuffer, 0);

    // Lines are already in world space, so the model matrix is identity.
    D3D11_MAPPED_SUBRESOURCE objMapped = {};
    context->Map(objectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &objMapped);
    auto* objBuffer = static_cast<ObjectBuffer*>(objMapped.pData);
    StoreTransposed(viewProj, objBuffer->worldViewProj);
    StoreTransposed(Matrix4::Identity(), objBuffer->world);
    context->Unmap(objectBuffer, 0);
    context->VSSetConstantBuffers(1, 1, &objectBuffer);

    UINT stride = sizeof(DebugVertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &debugVertexBuffer, &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    context->IASetInputLayout(debugShader.inputLayout);
    context->VSSetShader(debugShader.vertexShader, nullptr, 0);
    context->PSSetShader(debugShader.pixelShader, nullptr, 0);

    context->Draw(static_cast<UINT>(lines.size()), 0);
}

void D3D11Renderer::ReleaseShader(ShaderProgram& shader) {
    SafeRelease(shader.vertexShader);
    SafeRelease(shader.pixelShader);
    SafeRelease(shader.inputLayout);
}

void D3D11Renderer::ReleaseMesh(MeshResource& mesh) {
    SafeRelease(mesh.vertexBuffer);
    SafeRelease(mesh.materialBuffer);
    SafeRelease(mesh.textureSRV);
    SafeRelease(mesh.normalMapSRV);
    SafeRelease(mesh.samplerState);
    ReleaseShader(mesh.shader);
}

void D3D11Renderer::Shutdown() {
    for (auto& [handle, mesh] : meshes)
        ReleaseMesh(mesh);
    meshes.clear();

    SafeRelease(debugVertexBuffer);
    ReleaseShader(debugShader);
    SafeRelease(shadowDSV);
    SafeRelease(shadowSRV);
    SafeRelease(shadowSampler);
    SafeRelease(defaultNormalSRV);
    SafeRelease(lightSRV);
    SafeRelease(lightBuffer);
    SafeRelease(objectBuffer);
    SafeRelease(depthView);
    SafeRelease(depthState);
    if (swapChain) swapChain->SetFullscreenState(FALSE, nullptr);
    SafeRelease(backBuffer);
    SafeRelease(swapChain);
    SafeRelease(context);
    SafeRelease(device);
}

} // namespace ap
