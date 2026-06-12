#include "anchorpoint/systems/collision_system.h"

#include <cmath>

#include "anchorpoint/components/transform.h"
#include "anchorpoint/core/engine.h"

namespace ap {

namespace {

void GetCellCoords(const Vector3& min, const Vector3& max, float cellSize,
                   int& minX, int& maxX, int& minZ, int& maxZ) {
    minX = static_cast<int>(min.x / cellSize);
    minZ = static_cast<int>(min.z / cellSize);
    maxX = static_cast<int>(max.x / cellSize);
    maxZ = static_cast<int>(max.z / cellSize);
}

bool Intersects(const Vector3& aMin, const Vector3& aMax,
                const Vector3& bMin, const Vector3& bMax) {
    return (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
           (aMin.y <= bMax.y && aMax.y >= bMin.y) &&
           (aMin.z <= bMax.z && aMax.z >= bMin.z);
}

// Pushes the dynamic collider out of the static box along the axis of least
// overlap, adjusting the owning transform.
void ResolveOverlap(Transform& transform, const AABBCollider& collider,
                    const Vector3& staticMin, const Vector3& staticMax,
                    const Vector3& staticSize) {
    const Vector3 firstCenter = (collider.min + collider.max) * 0.5f;
    const Vector3 secondCenter = (staticMin + staticMax) * 0.5f;

    const Vector3 delta = firstCenter - secondCenter;
    const Vector3 totalHalfSize = (staticSize * 0.5f) + (collider.size * 0.5f);

    const Vector3 overlap = totalHalfSize
        - Vector3{std::abs(delta.x), std::abs(delta.y), std::abs(delta.z)};

    if (overlap.x > 0 && overlap.y > 0 && overlap.z > 0) {
        if (overlap.x < overlap.y && overlap.x < overlap.z) {
            transform.position.x += (delta.x < 0) ? -overlap.x : overlap.x;
        } else if (overlap.y < overlap.z) {
            transform.position.y += (delta.y < 0) ? -overlap.y : overlap.y;
        } else {
            transform.position.z += (delta.z < 0) ? -overlap.z : overlap.z;
        }
    }
}

} // namespace

void CollisionSystem::AddStatic(const StaticBox& box, int gx, int gz) {
    if (gx < 0 || gx >= kGridSize || gz < 0 || gz >= kGridSize) return;
    grid[CellIndex(gx, gz)].staticGeometry.push_back(box);
}

void CollisionSystem::QueryNearby(int minX, int maxX, int minZ, int maxZ,
                                  std::vector<const StaticBox*>& out) const {
    for (int gz = minZ; gz <= maxZ; ++gz) {
        for (int gx = minX; gx <= maxX; ++gx) {
            if (gx < 0 || gx >= kGridSize || gz < 0 || gz >= kGridSize) continue;
            for (const auto& box : grid[CellIndex(gx, gz)].staticGeometry)
                out.push_back(&box);
        }
    }
}

void CollisionSystem::OnUpdate(Engine& engine) {
    auto& world = engine.GetWorld();
    std::vector<const StaticBox*> nearby;

    world.Each<Transform, AABBCollider>([&](Entity entity, Transform& transform, AABBCollider& collider) {
        const Vector3 center = transform.position + collider.offset;
        collider.min = center + collider.size * -0.5f;
        collider.max = center + collider.size * 0.5f;

        int minX, maxX, minZ, maxZ;
        GetCellCoords(collider.min, collider.max, kCellSize, minX, maxX, minZ, maxZ);

        if (collider.isStatic) {
            if (!collider.registered) {
                AddStatic({entity, collider.min, collider.max, collider.size}, minX, minZ);
                collider.registered = true;
            }
            return;
        }

        nearby.clear();
        QueryNearby(minX, maxX, minZ, maxZ, nearby);
        for (const StaticBox* box : nearby) {
            if (box->entity == entity) continue;
            if (Intersects(collider.min, collider.max, box->min, box->max)) {
                ResolveOverlap(transform, collider, box->min, box->max, box->size);
                const Vector3 newCenter = transform.position + collider.offset;
                collider.min = newCenter + collider.size * -0.5f;
                collider.max = newCenter + collider.size * 0.5f;
            }
        }
    });
}

} // namespace ap
