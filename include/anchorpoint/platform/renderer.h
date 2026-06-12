#ifndef ANCHORPOINT_PLATFORM_RENDERER_H
#define ANCHORPOINT_PLATFORM_RENDERER_H

#include <cstdint>
#include <memory>
#include <vector>

#include "anchorpoint/assets/mesh.h"
#include "anchorpoint/core/api.h"
#include "anchorpoint/core/config.h"
#include "anchorpoint/math/math.h"
#include "anchorpoint/platform/window.h"

namespace ap {

using MeshHandle = std::uint32_t;
constexpr MeshHandle kInvalidMesh = 0;

// GPU light layout; must match the Light struct in the pixel shaders
// (48 bytes, tightly packed).
struct GpuLight {
    std::int32_t type = 0; // 0 directional, 1 point, 2 spot
    Vector3 position;
    float range = 0;
    Vector3 direction;
    Vector3 color;
    float intensity = 0;
};

struct DrawItem {
    MeshHandle mesh = kInvalidMesh;
    Matrix4 world;
};

// Everything a backend needs to draw one frame, gathered by the
// RenderSystem from the World each frame.
struct FrameData {
    Matrix4 view;
    Matrix4 projection;
    float clearColor[4] = {0, 0, 0, 1};
    std::vector<GpuLight> lights;
    std::vector<DrawItem> items;
};

// Abstract rendering backend. The library ships a Direct3D 11 backend on
// Windows and a null backend for headless use; implement this interface to
// plug in OpenGL/Vulkan/Metal backends on other platforms.
class Renderer {
public:
    virtual ~Renderer() = default;

    virtual bool Init(Window& window, const EngineConfig& config) = 0;
    virtual MeshHandle CreateMesh(const MeshData& data) = 0;
    virtual void RenderFrame(const FrameData& frame) = 0;
    virtual void Shutdown() = 0;
};

// Creates the renderer backend for the current platform (or the null
// backend when config.headless is set or no native backend exists).
AP_API std::unique_ptr<Renderer> CreatePlatformRenderer(const EngineConfig& config);

} // namespace ap

#endif //ANCHORPOINT_PLATFORM_RENDERER_H
