#include "game/components/transform.h"

std::string Transform::getName() {
    return "Transform";
}

void Transform::Update() {
}

void Transform::Destroy() {
}

void Transform::Init(nlohmann::basic_json<> data) {
    position[0] = data["position"][0];
    position[1] = data["position"][1];
    position[2] = data["position"][2];

    rotation[0] = data["rotation"][0];
    rotation[1] = data["rotation"][1];
    rotation[2] = data["rotation"][2];

    scale[0] = data["scale"][0];
    scale[1] = data["scale"][1];
    scale[2] = data["scale"][2];
}