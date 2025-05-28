#ifndef PROJECTANCHORPOINT_LIGHTCOMPONENT_H
#define PROJECTANCHORPOINT_LIGHTCOMPONENT_H

#include "game/components/component.h"
#include "game/components/transform.h"
#include "graphics/light.h"

class LightComponent : public Component {
public:
    Light light;
    Transform transform;

    std::string getName() override;

    void Init(nlohmann::basic_json<> data) override;

    void Update() override;

    void Destroy() override;
};

REGISTER_COMPONENT(LightComponent);

#endif //PROJECTANCHORPOINT_LIGHTCOMPONENT_H
