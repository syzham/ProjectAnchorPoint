#include "game/components/lightcomponent.h"

void LightComponent::Init(nlohmann::basic_json<> data) {
    transform = owner->getComponent<Transform>();
    light.type = data["lightType"];
    light.range = data["range"];
    light.intensity = data["intensity"];
    light.color.x = data["color"][0];
    light.color.y = data["color"][1];
    light.color.z = data["color"][2];

    light.position = transform->position;

    light.direction = transform->rotation;

    Lights::activeLights.push_back(light);
}

void LightComponent::Destroy() {
}

void LightComponent::Update() {
}




