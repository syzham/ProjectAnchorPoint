#ifndef ANCHORPOINT_COMPONENTS_CAMERA_H
#define ANCHORPOINT_COMPONENTS_CAMERA_H

#include "anchorpoint/math/math.h"

namespace ap {

// Attach next to a Transform; the RenderSystem uses the first active camera
// it finds. Yaw/pitch come from the Transform's rotation (x = yaw, y = pitch).
struct Camera {
    float fov = kPiDiv4;
    float nearZ = 0.1f;
    float farZ = 100.0f;
    bool active = false;
};

} // namespace ap

#endif //ANCHORPOINT_COMPONENTS_CAMERA_H
