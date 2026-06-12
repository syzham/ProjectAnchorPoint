#include "game/components/transform.h"

void Transform::AddRotation(Vector3 d, bool yClamped) {
    rotation += d;

    auto wrap = [](float& v) {
        if (v > DirectX::XM_PI)  v -= DirectX::XM_2PI;
        if (v < -DirectX::XM_PI) v += DirectX::XM_2PI;
    };
    wrap(rotation.x);
    wrap(rotation.y);
    wrap(rotation.z);

    if (yClamped)
        rotation.y = std::clamp(rotation.y, -DirectX::XM_PIDIV2, DirectX::XM_PIDIV2);
}

void Transform::MoveForwards(float speed) {
    AddPosition({std::sin(rotation.x) * speed, 0.0f, std::cos(rotation.x) * speed});
}

void Transform::MoveRight(float speed) {
    AddPosition({std::cos(rotation.x) * speed, 0.0f, -std::sin(rotation.x) * speed});
}

DirectX::XMMATRIX Transform::WorldMatrix() const {
    DirectX::XMMATRIX t   = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    DirectX::XMMATRIX rx  = DirectX::XMMatrixRotationX(rotation.x);
    DirectX::XMMATRIX ry  = DirectX::XMMatrixRotationY(rotation.y);
    DirectX::XMMATRIX rz  = DirectX::XMMatrixRotationZ(rotation.z);
    DirectX::XMMATRIX rot = rz * ry * rx;
    DirectX::XMMATRIX s   = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    return s * rot * t;
}
