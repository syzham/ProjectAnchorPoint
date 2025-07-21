#ifndef PROJECTANCHORPOINT_TRANSFORM_H
#define PROJECTANCHORPOINT_TRANSFORM_H

#include "game/components/component.h"
#include "common.h"

class Transform : public Component {
public:
    Vector3 position = {0, 0, 0};
    Vector3 rotation = {0, 0, 0};
    Vector3 scale = {0, 0, 0};

    void Init(nlohmann::basic_json<> data) override;

    void Update() override;

    void Destroy() override;

    void SetPosition(Vector3 newPos);

    void SetRotation(Vector3 newRot);

    void SetScale(Vector3 newScale);

    void AddPosition(Vector3 addPos);

    void AddRotation(Vector3 addRot, bool yClamped=false);

    void AddScale(Vector3 addScale);

    void MoveForwards(float speed);

    void MoveRight(float speed);
};

REGISTER_COMPONENT(Transform);

#endif //PROJECTANCHORPOINT_TRANSFORM_H
