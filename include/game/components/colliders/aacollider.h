#ifndef PROJECTANCHORPOINT_AACOLLIDER_H
#define PROJECTANCHORPOINT_AACOLLIDER_H

#include "game/components/colliders/collidercomponent.h"

class AACollider : public ColliderComponent {
public:
    Vector3 max, min, size;

    void Init(nlohmann::basic_json<> data) override;

    void UpdatePosition() override;

    void GetCoords(int &minX, int &maxX, int &minZ, int &maxZ) const override;

    bool Intersects(const ColliderComponent& other) const override;
    bool IntersectsWithAA(const AACollider& other) const override;

    void ResolveOverlap(ColliderComponent& other) override;
    void ResolveWithAA(const AACollider& other) override;
};

REGISTER_COMPONENT(AACollider);

#endif //PROJECTANCHORPOINT_AACOLLIDER_H
