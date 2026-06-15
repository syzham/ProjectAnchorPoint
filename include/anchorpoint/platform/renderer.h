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

// A single vertex of a debug line list (world-space position + RGB colour),
// drawn unlit and untextured. Two consecutive vertices form one segment.
struct DebugVertex {
    Vector3 position;
    Vector3 color;
};

// Everything a backend needs to draw one frame, gathered by the
// RenderSystem from the World each frame.
struct FrameData {
    Matrix4 view;
    Matrix4 projection;
    float clearColor[4] = {0, 0, 0, 1};
    std::vector<GpuLight> lights;
    std::vector<DrawItem> items;
    std::vector<DebugVertex> debugLines;
};

// Appends the 12 edges (24 vertices) of an axis-aligned box to a debug line
// list. Reusable for any debug visualisation, not just colliders.
inline void AppendAABBWireframe(std::vector<DebugVertex>& out,
                                const Vector3& min, const Vector3& max,
                                const Vector3& color) {
    const Vector3 corners[8] = {
        {min.x, min.y, min.z}, {max.x, min.y, min.z},
        {max.x, min.y, max.z}, {min.x, min.y, max.z},
        {min.x, max.y, min.z}, {max.x, max.y, min.z},
        {max.x, max.y, max.z}, {min.x, max.y, max.z},
    };
    // Bottom face, top face, then the four vertical edges.
    const int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7},
    };
    for (const auto& edge : edges) {
        out.push_back({corners[edge[0]], color});
        out.push_back({corners[edge[1]], color});
    }
}

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
