#ifndef ANCHORPOINT_COMPONENTS_COLLIDER_H
#define ANCHORPOINT_COMPONENTS_COLLIDER_H

#include "anchorpoint/math/math.h"

namespace ap {

// Axis-aligned box collider. Static colliders are registered once in the
// CollisionSystem's spatial grid; dynamic colliders are resolved against
// static geometry every update.
struct AABBCollider {
    // Box centre relative to the owning Transform's position. For a character
    // whose Transform sits at eye height, offset the collider down by half its
    // height so the box covers the body; otherwise only the lower edge overlaps
    // world geometry and the minimum-translation response pushes the character
    // up and over obstacles instead of blocking it.
    Vector3 offset = {0, 0, 0};
    Vector3 size = {1, 1, 1};
    bool isStatic = false;

    // Maintained by the CollisionSystem.
    Vector3 min, max;
    bool registered = false;
};

} // namespace ap

#endif //ANCHORPOINT_COMPONENTS_COLLIDER_H
