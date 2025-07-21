#include "game/components/cameracomponent.h"

void CameraComponent::Init(nlohmann::basic_json<> data) {
    transform = owner->getComponent<Transform>();
    if (data.contains("active")) currentActive = true;
}

void CameraComponent::Update(){
    if (!currentActive) return;
    Camera::getMainCamera().SetPosition(transform->position);
    Camera::getMainCamera().LookAt(transform->rotation);
}

void CameraComponent::Destroy(){
}