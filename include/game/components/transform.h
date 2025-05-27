#ifndef PROJECTANCHORPOINT_TRANSFORM_H
#define PROJECTANCHORPOINT_TRANSFORM_H

#include "game/components/component.h"

class Transform : public Component {
public:
    float position[3] = {0, 0, 0};
    float rotation[3] = {0, 0, 0};
    float scale[3] = {0, 0, 0};

    std::string getName() override;

    void Init(nlohmann::basic_json<> data) override;

    void Update() override;

    void Destroy() override;
};

REGISTER_COMPONENT(Transform);

#endif //PROJECTANCHORPOINT_TRANSFORM_H
