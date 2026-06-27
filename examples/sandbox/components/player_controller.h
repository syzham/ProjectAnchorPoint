#ifndef SANDBOX_COMPONENTS_PLAYER_CONTROLLER_H
#define SANDBOX_COMPONENTS_PLAYER_CONTROLLER_H

namespace sandbox {

// Tuning data for the first-person controller; behaviour lives in
// systems/player_controller_system.h.
struct PlayerController {
    float sensitivity = 0.001f;
    float speed = 9.0f;
};

} // namespace sandbox

#endif //SANDBOX_COMPONENTS_PLAYER_CONTROLLER_H
