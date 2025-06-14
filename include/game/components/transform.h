#ifndef PROJECTANCHORPOINT_TRANSFORM_H
#define PROJECTANCHORPOINT_TRANSFORM_H

#include "game/components/component.h"

class Transform : public Component {
public:
    float position[3] = {0, 0, 0};
    float rotation[3] = {0, 0, 0};
    float scale[3] = {0, 0, 0};

    void Init(nlohmann::basic_json<> data) override;

    void Update() override;

    void Destroy() override;

    void SetPosition(float x, float y, float z);

    void SetRotation(float x, float y, float z);

    void SetScale(float x, float y, float z);

    void AddPosition(float x, float y, float z);

    void AddRotation(float x, float y, float z, bool yClamped=false);

    void AddScale(float x, float y, float z);
};

REGISTER_COMPONENT(Transform);

#endif //PROJECTANCHORPOINT_TRANSFORM_H
