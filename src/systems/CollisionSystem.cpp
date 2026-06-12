#include "systems/CollisionSystem.h"
#include <cmath>

void CollisionSystem::UpdateBounds(const Transform& t, AABBCollider& col) {
    Vector3 center = t.position + col.offset;
    col.min = center + Vector3{-col.size.x * 0.5f, -col.size.y * 0.5f, -col.size.z * 0.5f};
    col.max = center + Vector3{ col.size.x * 0.5f,  col.size.y * 0.5f,  col.size.z * 0.5f};
}

bool CollisionSystem::Intersects(const AABBCollider& a, const AABBCollider& b) {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
           (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

void CollisionSystem::Resolve(Transform& t, const AABBCollider& col, const AABBCollider& other) {
    Vector3 fc = {(col.min.x + col.max.x) * 0.5f,
                  (col.min.y + col.max.y) * 0.5f,
                  (col.min.z + col.max.z) * 0.5f};
    Vector3 sc = {(other.min.x + other.max.x) * 0.5f,
                  (other.min.y + other.max.y) * 0.5f,
                  (other.min.z + other.max.z) * 0.5f};
    Vector3 delta      = fc - sc;
    Vector3 totalHalf  = (col.size + other.size) * 0.5f;
    Vector3 overlap    = totalHalf - Vector3{std::fabsf(delta.x), std::fabsf(delta.y), std::fabsf(delta.z)};

    if (overlap.x <= 0.0f || overlap.y <= 0.0f || overlap.z <= 0.0f) return;

    if (overlap.x < overlap.y && overlap.x < overlap.z)
        t.position.x += (delta.x < 0.0f) ? -overlap.x : overlap.x;
    else if (overlap.y < overlap.z)
        t.position.y += (delta.y < 0.0f) ? -overlap.y : overlap.y;
    else
        t.position.z += (delta.z < 0.0f) ? -overlap.z : overlap.z;
}

void CollisionSystem::Update(Registry& registry, float dt) {
    // Update all AABB bounds from current transform positions.
    registry.forEach<Transform, AABBCollider>([](Entity e, Transform& t, AABBCollider& col) {
        UpdateBounds(t, col);
    });

    // Resolve dynamic vs. any collider (static or dynamic).
    registry.forEach<Transform, AABBCollider>([&](Entity a, Transform& ta, AABBCollider& ca) {
        if (ca.isStatic) return;

        registry.forEach<Transform, AABBCollider>([&](Entity b, Transform& tb, AABBCollider& cb) {
            if (a == b) return;
            if (Intersects(ca, cb))
                Resolve(ta, ca, cb);
        });
    });
}
