#ifndef ANCHORPOINT_PLATFORM_METAL_RENDERER_H
#define ANCHORPOINT_PLATFORM_METAL_RENDERER_H

#include <memory>

#include "anchorpoint/platform/renderer.h"

namespace ap {

// Metal rendering backend for macOS. As with the Cocoa window, all
// Objective-C / Metal state is hidden behind an Impl defined in the .mm so
// this header stays pure C++.
class MetalRenderer final : public Renderer {
public:
    MetalRenderer();
    ~MetalRenderer() override;

    bool Init(Window& window, const EngineConfig& config) override;
    MeshHandle CreateMesh(const MeshData& data) override;
    void RenderFrame(const FrameData& frame) override;
    void Shutdown() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace ap

#endif //ANCHORPOINT_PLATFORM_METAL_RENDERER_H
