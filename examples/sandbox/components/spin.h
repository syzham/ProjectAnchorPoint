#ifndef SANDBOX_COMPONENTS_SPIN_H
#define SANDBOX_COMPONENTS_SPIN_H

#include <anchorpoint/math/math.h>

namespace sandbox {

// Per-frame rotation applied by systems/spin_system.h.
struct Spin {
    ap::Vector3 delta;
};

} // namespace sandbox

#endif //SANDBOX_COMPONENTS_SPIN_H
