#include "graphics/camera.h"

DirectX::XMMATRIX Camera::GetProjectionMatrix() const {
    return DirectX::XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);
}

DirectX::XMMATRIX Camera::GetViewMatrix() const {
    return DirectX::XMMatrixLookAtLH(position, target, up);
}

void Camera::SetPerspective(float newFov, float newAspect, float newNearZ, float newFarZ) {
    fov = newFov;
    aspect = newAspect;
    nearZ = newNearZ;
    farZ = newFarZ;
}

void Camera::LookAt(float x, float y, float z) {
    target = DirectX::XMVectorSet(x, y, z, 1);
}

void Camera::SetPosition(float x, float y, float z) {
    position = DirectX::XMVectorSet(x, y, z, 1);
}

void Camera::Move(float x, float y, float z) {
    position = DirectX::XMVectorAdd(position, DirectX::XMVectorSet(x, y, z, 1));
}
