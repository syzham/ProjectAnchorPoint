#include "game/components/transform.h"

void Transform::Update() {
}

void Transform::Destroy() {
}

void Transform::Init(nlohmann::basic_json<> data) {
    position.x = data["position"][0];
    position.y = data["position"][1];
    position.z = data["position"][2];

    rotation.x = data["rotation"][0];
    rotation.y = data["rotation"][1];
    rotation.z = data["rotation"][2];

    scale.x = data["scale"][0];
    scale.y = data["scale"][1];
    scale.z = data["scale"][2];
}

void Transform::SetPosition(Vector3 newPos) {
    position = newPos;
}

void Transform::SetRotation(Vector3 newRot) {
    rotation = newRot;
}

void Transform::SetScale(Vector3 newScale) {
    scale = newScale;
}

void Transform::AddPosition(Vector3 addPos) {
    position += addPos;
}

void Transform::AddRotation(Vector3 addRot, bool yClamped) {
    rotation += addRot;

    if (rotation.x > DirectX::XM_PI) rotation.x -= DirectX::XM_2PI;
    if (rotation.x < -DirectX::XM_PI) rotation.x += DirectX::XM_2PI;

    if (rotation.y > DirectX::XM_PI) rotation.y -= DirectX::XM_2PI;
    if (rotation.y < -DirectX::XM_PI) rotation.y += DirectX::XM_2PI;

    if (rotation.z > DirectX::XM_PI) rotation.z -= DirectX::XM_2PI;
    if (rotation.z < -DirectX::XM_PI) rotation.z += DirectX::XM_2PI;

    if (yClamped) {
        rotation.y = std::clamp(rotation.y, -DirectX::XM_PI / 2, DirectX::XM_PI / 2);
    }
}

void Transform::AddScale(Vector3 addScale) {
    scale += addScale;
}

void Transform::MoveForwards(float speed) {
    AddPosition({sin(rotation.x) * speed, 0, cos(rotation.x) * speed});
}

void Transform::MoveRight(float speed) {
    AddPosition({cos(rotation.x) * speed, 0, -sin(rotation.x) * speed});
}