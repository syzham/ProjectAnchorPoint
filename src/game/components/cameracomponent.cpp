#include "game/components/cameracomponent.h"

void CameraComponent::Init(nlohmann::basic_json<> data) {
    transform = owner->getComponent<Transform>();
    if (data.contains("active")) currentActive = true;
}

void CameraComponent::Update(){
    if (!currentActive) return;
    Camera::getMainCamera().SetPosition(transform->position[0], transform->position[1], transform->position[2]);
    Camera::getMainCamera().LookAt(transform->rotation[0], transform->rotation[1], transform->rotation[2]);
}

void CameraComponent::Destroy(){
}