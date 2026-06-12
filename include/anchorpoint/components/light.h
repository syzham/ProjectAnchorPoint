#ifndef ANCHORPOINT_COMPONENTS_LIGHT_H
#define ANCHORPOINT_COMPONENTS_LIGHT_H

#include <cstdint>

#include "anchorpoint/math/math.h"

namespace ap {

enum class LightType : std::int32_t {
    Directional = 0,
    Point = 1,
    Spot = 2,
};

// Attach next to a Transform; position comes from the Transform's position
// and direction from its rotation vector.
struct Light {
    LightType type = LightType::Directional;
    Vector3 color = {1, 1, 1};
    float intensity = 1.0f;
    float range = 1.0f;
};

} // namespace ap

#endif //ANCHORPOINT_COMPONENTS_LIGHT_H
