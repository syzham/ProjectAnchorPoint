#include "game/components/lightcomponent.h"

void LightComponent::Init(nlohmann::basic_json<> data) {
    transform = owner->getComponent<Transform>();
    light.type = data["lightType"];
    light.range = data["range"];
    light.intensity = data["intensity"];
    light.color[0] = data["color"][0];
    light.color[1] = data["color"][1];
    light.color[2] = data["color"][2];

    light.position[0] = transform->position[0];
    light.position[1] = transform->position[1];
    light.position[2] = transform->position[2];

    light.direction[0] = transform->rotation[0];
    light.direction[1] = transform->rotation[1];
    light.direction[2] = transform->rotation[2];

    Lights::activeLights.push_back(std::move(light));
}

void LightComponent::Destroy() {
}

void LightComponent::Update() {
}




