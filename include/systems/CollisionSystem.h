#pragma once

#include "core/System.h"
#include "game/components/transform.h"
#include "game/components/colliders/aacollider.h"

class CollisionSystem : public ISystem {
public:
    void Update(Registry& registry, float dt) override;

private:
    static void UpdateBounds(const Transform& t, AABBCollider& col);
    static bool Intersects(const AABBCollider& a, const AABBCollider& b);
    static void Resolve(Transform& t, const AABBCollider& col, const AABBCollider& other);
};
