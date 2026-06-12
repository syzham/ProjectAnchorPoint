#include "systems/RenderSystem.h"

void RenderSystem::Update(Registry& registry, float dt) {
    registry.forEach<Transform, MeshRenderer>([](Entity e, Transform& t, MeshRenderer& mr) {
        if (!mr.initialized) return;
        mr.mesh.Draw(t.WorldMatrix());
    });
}
