#include "game/components/colliders/collidercomponent.h"

void ColliderComponent::Update() {
    if (staticObject) return;
    UpdatePosition();
    int minX, maxX, minZ, maxZ;
    GetCoords(minX, maxX, minZ, maxZ);
    for (auto* box : StaticWorld::getInstance().QueryNearby(minX, maxX, minZ, maxZ)) {
        if (IntersectsWithAA(*box)) {
            ResolveWithAA(*box);
        }
    }
}

void ColliderComponent::Destroy(){
}