#include "systems/CameraSystem.h"

void CameraSystem::Update(Registry& registry, float dt) {
    registry.forEach<Transform, CameraComp>([](Entity e, Transform& t, CameraComp& cam) {
        if (!cam.isActive) return;
        Camera::getMainCamera().SetPosition(t.position);
        Camera::getMainCamera().LookAt(t.rotation);
    });
}
