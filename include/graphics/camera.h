#ifndef PROJECTANCHORPOINT_CAMERA_H
#define PROJECTANCHORPOINT_CAMERA_H

#include <DirectXMath.h>
#include "common.h"

class Camera {
public:
    static Camera& getMainCamera();

    void SetPerspective(float fov, float aspect, float nearZ, float farZ);
    void SetPosition(Vector3 newPos);
    void LookAt(Vector3 newRotation);
    void Move(Vector3 addPos);
    void Pan(Vector3 addRotation);

    DirectX::XMMATRIX GetViewMatrix() const;
    DirectX::XMMATRIX GetProjectionMatrix() const;

private:
    DirectX::XMVECTOR position = DirectX::XMVectorSet(0.0f, 0.0f, -3.0f, 1.0f);
    DirectX::XMVECTOR rotation = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0, 1.0f, 0.0f, 0.0f);

    float fov = DirectX::XM_PIDIV4;
    float aspect = 800.0f / 600.0f;
    float nearZ = 0.1f;
    float farZ = 100.0;
};

#endif //PROJECTANCHORPOINT_CAMERA_H
