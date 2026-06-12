#ifndef ANCHORPOINT_COMPONENTS_COLLIDER_H
#define ANCHORPOINT_COMPONENTS_COLLIDER_H

#include "anchorpoint/math/math.h"

namespace ap {

// Axis-aligned box collider. Static colliders are registered once in the
// CollisionSystem's spatial grid; dynamic colliders are resolved against
// static geometry every update.
struct AABBCollider {
    Vector3 offset = {0, 0, 0};
    Vector3 size = {1, 1, 1};
    bool isStatic = false;

    // Maintained by the CollisionSystem.
    Vector3 min, max;
    bool registered = false;
};

} // namespace ap

#endif //ANCHORPOINT_COMPONENTS_COLLIDER_H
