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

void Camera::LookAt(float x, float y, float z) {
    rotation = DirectX::XMVectorSet(x, y, z, 1);
}

void Camera::SetPosition(float x, float y, float z) {
    position = DirectX::XMVectorSet(x, y, z, 1);
}

void Camera::Move(float x, float y, float z) {
    position = DirectX::XMVectorAdd(position, DirectX::XMVectorSet(x, y, z, 1));
}

void Camera::Pan(float x, float y, float z) {
    rotation = DirectX::XMVectorAdd(rotation, DirectX::XMVectorSet(x, y, z, 1));
}

Camera& Camera::getMainCamera() {
    static Camera main;
    return main;
}