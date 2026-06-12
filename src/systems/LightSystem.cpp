#include "systems/LightSystem.h"

void LightSystem::Update(Registry& registry, float dt) {
    Lights::activeLights.clear();

    registry.forEach<Transform, LightComp>([](Entity e, Transform& t, LightComp& lc) {
        lc.light.position  = t.position;
        lc.light.direction = t.rotation;
        Lights::activeLights.push_back(lc.light);
    });

    Lights::Update();
}
