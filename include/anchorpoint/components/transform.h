#ifndef ANCHORPOINT_COMPONENTS_TRANSFORM_H
#define ANCHORPOINT_COMPONENTS_TRANSFORM_H

#include <algorithm>

#include "anchorpoint/math/math.h"

namespace ap {

// rotation is in radians: x = yaw, y = pitch, z = roll.
struct Transform {
    Vector3 position = {0, 0, 0};
    Vector3 rotation = {0, 0, 0};
    Vector3 scale = {1, 1, 1};
};

// Components are plain data, so transform behaviour lives in free functions.

inline void AddRotation(Transform& transform, Vector3 delta, bool pitchClamped = false) {
    transform.rotation += delta;

    auto wrap = [](float& angle) {
        if (angle > kPi) angle -= kTwoPi;
        if (angle < -kPi) angle += kTwoPi;
    };
    wrap(transform.rotation.x);
    wrap(transform.rotation.y);
    wrap(transform.rotation.z);

    if (pitchClamped)
        transform.rotation.y = std::clamp(transform.rotation.y, -kPiDiv2, kPiDiv2);
}

inline void MoveForwards(Transform& transform, float distance) {
    transform.position += {std::sin(transform.rotation.x) * distance, 0,
                           std::cos(transform.rotation.x) * distance};
}

inline void MoveRight(Transform& transform, float distance) {
    transform.position += {std::cos(transform.rotation.x) * distance, 0,
                           -std::sin(transform.rotation.x) * distance};
}

inline Matrix4 WorldMatrix(const Transform& transform) {
    const Matrix4 rotation = Matrix4::RotationZ(transform.rotation.z)
                           * Matrix4::RotationY(transform.rotation.y)
                           * Matrix4::RotationX(transform.rotation.x);
    return Matrix4::Scaling(transform.scale)
         * rotation
         * Matrix4::Translation(transform.position);
}

} // namespace ap

#endif //ANCHORPOINT_COMPONENTS_TRANSFORM_H
