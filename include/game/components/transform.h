#pragma once

#include "common.h"
#include <DirectXMath.h>
#include <cmath>
#include <algorithm>

struct Transform {
    Vector3 position = {0.0f, 0.0f, 0.0f};
    Vector3 rotation = {0.0f, 0.0f, 0.0f};
    Vector3 scale    = {1.0f, 1.0f, 1.0f};

    void SetPosition(Vector3 p) { position = p; }
    void SetRotation(Vector3 r) { rotation = r; }
    void SetScale(Vector3 s)    { scale = s; }

    void AddPosition(Vector3 d) { position += d; }
    void AddScale(Vector3 d)    { scale += d; }

    void AddRotation(Vector3 d, bool yClamped = false);
    void MoveForwards(float speed);
    void MoveRight(float speed);

    DirectX::XMMATRIX WorldMatrix() const;
};
