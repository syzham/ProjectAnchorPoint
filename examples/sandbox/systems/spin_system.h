#ifndef SANDBOX_SYSTEMS_SPIN_SYSTEM_H
#define SANDBOX_SYSTEMS_SPIN_SYSTEM_H

#include <anchorpoint/components/transform.h>
#include <anchorpoint/core/engine.h>

#include "components/spin.h"
#include "game/state_system.h"

namespace sandbox {

// Rotates every entity with a Spin component. Runs only while Playing.
class SpinSystem : public StateSystem {
public:
    SpinSystem() : StateSystem({GameState::Playing}) {}

protected:
    void OnUpdateInState(ap::Engine& engine) override {
        engine.GetWorld().Each<ap::Transform, Spin>(
            [](ap::Entity, ap::Transform& transform, Spin& spin) {
                ap::AddRotation(transform, spin.delta);
            });
    }
};

} // namespace sandbox

#endif //SANDBOX_SYSTEMS_SPIN_SYSTEM_H
