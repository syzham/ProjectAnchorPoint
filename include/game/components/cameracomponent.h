#ifndef PROJECTANCHORPOINT_CAMERACOMPONENT_H
#define PROJECTANCHORPOINT_CAMERACOMPONENT_H

#include "game/components/component.h"
#include "game/components/transform.h"

class CameraComponent : public Component {
    Transform* transform;
    bool currentActive = false;

    void Init(nlohmann::basic_json<> data) override;

    void Update() override;

    void Destroy() override;
};

REGISTER_COMPONENT(CameraComponent);

#endif //PROJECTANCHORPOINT_CAMERACOMPONENT_H
