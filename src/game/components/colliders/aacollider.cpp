#include "game/components/colliders/aacollider.h"
#include "game/CollisionManager.h"

void AACollider::Init(nlohmann::basic_json<> data) {
    transform = owner->getComponent<Transform>();
    offset.x = data["offset"][0];
    offset.y = data["offset"][1];
    offset.z = data["offset"][2];

    size.x = data["size"][0];
    size.y = data["size"][1];
    size.z = data["size"][2];

    staticObject = data["static"];

    Vector3 center = transform->position + offset;

    min = center + Vector3(-size.x / 2, -size.y / 2, -size.z / 2);
    max = center + Vector3(size.x / 2, size.y / 2, size.z / 2);

    if (staticObject) {
        int minX, maxX, minZ, maxZ;
        GetCoords(minX, maxX, minZ, maxZ);
        StaticWorld::getInstance().AddStatic(this, minX, minZ);
    }
}

void AACollider::UpdatePosition() {
    Vector3 center = transform->position + offset;
    min = center + Vector3(-size.x / 2, -size.y / 2, -size.z / 2);
    max = center + Vector3(size.x / 2, size.y / 2, size.z / 2);
}

void AACollider::GetCoords(int &minX, int &maxX, int &minZ, int &maxZ) const{
    minX = static_cast<int>(min.x / CELL_SIZE);
    minZ = static_cast<int>(min.z / CELL_SIZE);
    maxX = static_cast<int>(max.x / CELL_SIZE);
    maxZ = static_cast<int>(max.z / CELL_SIZE);
}

bool AACollider::Intersects(const ColliderComponent& other) const {
    return other.IntersectsWithAA(*this);
}

bool AACollider::IntersectsWithAA(const AACollider& other) const {
    return (min.x <= other.max.x && max.x >= other.min.x) &&
           (min.y <= other.max.y && max.y >= other.min.y) &&
           (min.z <= other.max.z && max.z >= other.min.z);
}

void AACollider::ResolveOverlap(ColliderComponent& other) {
    return other.ResolveWithAA(*this);
}

void AACollider::ResolveWithAA(const AACollider& other) {
    Vector3 firstCenter = (min + max) * 0.5;
    Vector3 secondCenter = (other.min + other.max) * 0.5;

    Vector3 delta = firstCenter - secondCenter;
    Vector3 totalHalfSize = (other.size * 0.5) + (size * 0.5);

    Vector3 overlap = totalHalfSize - Vector3(abs(delta.x), abs(delta.y), abs(delta.z));

    if (overlap.x > 0 && overlap.y > 0 && overlap.z > 0) {
        if (overlap.x < overlap.y && overlap.x < overlap.z) {
            float push = (delta.x < 0) ? -overlap.x : overlap.x;
            transform->position.x += push;
        }
        else if (overlap.y < overlap.z) {
            float push = (delta.y < 0) ? -overlap.y : overlap.y;
            transform->position.y += push;
        }
        else {
            float push = (delta.z < 0) ? -overlap.z : overlap.z;
            transform->position.z += push;
        }
    }
}
