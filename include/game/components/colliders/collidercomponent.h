#ifndef PROJECTANCHORPOINT_COLLIDERCOMPONENT_H
#define PROJECTANCHORPOINT_COLLIDERCOMPONENT_H

#include "game/components/transform.h"
#include "game/CollisionManager.h"

class AACollider;

class ColliderComponent : public Component {
public:
    Transform* transform;
    Vector3 offset;
    bool staticObject;

    void Update() override;

    void Destroy() override;

    virtual void UpdatePosition() = 0;

    virtual void GetCoords(int &minX, int &maxX, int &minZ, int &maxZ) const = 0;

    virtual bool Intersects(const ColliderComponent& other) const = 0;
    virtual bool IntersectsWithAA(const AACollider& other) const = 0;

    virtual void ResolveOverlap(ColliderComponent& other) = 0;
    virtual void ResolveWithAA(const AACollider& other) = 0;
};

#endif //PROJECTANCHORPOINT_COLLIDERCOMPONENT_H
