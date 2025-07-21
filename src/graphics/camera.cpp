#include <algorithm>
#include "graphics/camera.h"

DirectX::XMMATRIX Camera::GetProjectionMatrix() const {
    return DirectX::XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);
}

DirectX::XMMATRIX Camera::GetViewMatrix() const {
    float pitch = rotation.m128_f32[1];
    float yaw = rotation.m128_f32[0];
    DirectX::XMVECTOR forward = DirectX::XMVectorSet(cosf(pitch) * sinf(yaw), sinf(pitch), cosf(pitch) * cosf(yaw), 0.0f);
    DirectX::XMVECTOR targetPosition = DirectX::XMVectorAdd(position, forward);
    return DirectX::XMMatrixLookAtLH(position, targetPosition, up);
}

void Camera::SetPerspective(float newFov, float newAspect, float newNearZ, float newFarZ) {
    fov = newFov;
    aspect = newAspect;
    nearZ = newNearZ;
    farZ = newFarZ;
}

void Camera::LookAt(Vector3 newRotation) {
    rotation = DirectX::XMVectorSet(newRotation.x, newRotation.y, newRotation.z, 1);
}

void Camera::SetPosition(Vector3 newPosition) {
    position = DirectX::XMVectorSet(newPosition.x, newPosition.y, newPosition.z, 1);
}

void Camera::Move(Vector3 addPosition) {
    position = DirectX::XMVectorAdd(position, DirectX::XMVectorSet(addPosition.x, addPosition.y, addPosition.z, 1));
}

void Camera::Pan(Vector3 addRotation) {
    rotation = DirectX::XMVectorAdd(rotation, DirectX::XMVectorSet(addRotation.x, addRotation.y, addRotation.z, 1));
}

Camera& Camera::getMainCamera() {
    static Camera main;
    return main;
}