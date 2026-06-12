#ifndef ANCHORPOINT_PLATFORM_NULL_PLATFORM_H
#define ANCHORPOINT_PLATFORM_NULL_PLATFORM_H

#include "anchorpoint/platform/renderer.h"
#include "anchorpoint/platform/window.h"

namespace ap {

// Headless backends: no window, no GPU. Used for servers, tests and CI, and
// as the fallback on platforms without a native rendering backend yet.

class NullWindow final : public Window {
public:
    bool Create(const EngineConfig& config, Input& input) override { return true; }
    void Show() override {}
    bool PumpEvents() override { return true; }
    void SetCursorCaptured(bool captured) override {}
    void* GetNativeHandle() const override { return nullptr; }
};

class NullRenderer final : public Renderer {
public:
    bool Init(Window& window, const EngineConfig& config) override { return true; }

    MeshHandle CreateMesh(const MeshData& data) override { return nextHandle++; }

    void RenderFrame(const FrameData& frame) override {}

    void Shutdown() override {}

private:
    MeshHandle nextHandle = 1;
};

} // namespace ap

#endif //ANCHORPOINT_PLATFORM_NULL_PLATFORM_H
