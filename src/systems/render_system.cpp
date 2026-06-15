#include "anchorpoint/systems/render_system.h"

#include "anchorpoint/components/camera.h"
#include "anchorpoint/components/collider.h"
#include "anchorpoint/components/light.h"
#include "anchorpoint/components/mesh_renderer.h"
#include "anchorpoint/components/transform.h"
#include "anchorpoint/core/engine.h"
#include "anchorpoint/core/log.h"

namespace ap {

void RenderSystem::OnUpdate(Engine& engine) {
    auto& world = engine.GetWorld();
    auto& renderer = engine.GetRenderer();
    const auto& config = engine.GetConfig();

    FrameData frame;
    for (int i = 0; i < 4; ++i)
        frame.clearColor[i] = config.clearColor[i];

    const float aspect = static_cast<float>(config.width) / static_cast<float>(config.height);

    // Use the first active camera; fall back to a default projection so a
    // scene without cameras still clears and presents.
    bool cameraFound = false;
    frame.projection = Matrix4::PerspectiveFovLH(kPiDiv4, aspect, 0.1f, 100.0f);
    world.Each<Transform, Camera>([&](Entity, Transform& transform, Camera& camera) {
        if (!camera.active || cameraFound) return;
        cameraFound = true;

        const float yaw = transform.rotation.x;
        const float pitch = transform.rotation.y;
        const Vector3 forward = {std::cos(pitch) * std::sin(yaw),
                                 std::sin(pitch),
                                 std::cos(pitch) * std::cos(yaw)};
        frame.view = Matrix4::LookAtLH(transform.position,
                                       transform.position + forward,
                                       {0, 1, 0});
        frame.projection = Matrix4::PerspectiveFovLH(camera.fov, aspect,
                                                     camera.nearZ, camera.farZ);
    });

    world.Each<Transform, Light>([&](Entity, Transform& transform, Light& light) {
        GpuLight gpuLight;
        gpuLight.type = static_cast<std::int32_t>(light.type);
        gpuLight.position = transform.position;
        gpuLight.range = light.range;
        gpuLight.direction = transform.rotation;
        gpuLight.color = light.color;
        gpuLight.intensity = light.intensity;
        frame.lights.push_back(gpuLight);
    });

    world.Each<Transform, MeshRenderer>([&](Entity, Transform& transform, MeshRenderer& meshRenderer) {
        if (meshRenderer.gpuMesh == kInvalidMesh && !meshRenderer.loadFailed) {
            MeshData data;
            data.topology = meshRenderer.topology;
            if (LoadOBJ(meshRenderer.model, data)) {
                meshRenderer.gpuMesh = renderer.CreateMesh(data);
            }
            if (meshRenderer.gpuMesh == kInvalidMesh) {
                meshRenderer.loadFailed = true;
                LogError(("Failed to create mesh: " + meshRenderer.model).c_str());
            }
        }
        if (meshRenderer.gpuMesh != kInvalidMesh)
            frame.items.push_back({meshRenderer.gpuMesh, WorldMatrix(transform)});
    });

    if (engine.IsDebugDrawColliders()) {
        const Vector3 staticColor = {0, 1, 0};
        const Vector3 dynamicColor = {1, 0, 0};
        world.Each<Transform, AABBCollider>([&](Entity, Transform& transform, AABBCollider& collider) {
            // Recompute the bounds here so the overlay is correct regardless of
            // whether the collision system has run this frame.
            const Vector3 center = transform.position + collider.offset;
            const Vector3 min = center + collider.size * -0.5f;
            const Vector3 max = center + collider.size * 0.5f;
            AppendAABBWireframe(frame.debugLines, min, max,
                                collider.isStatic ? staticColor : dynamicColor);
        });
    }

    renderer.RenderFrame(frame);
}

} // namespace ap
