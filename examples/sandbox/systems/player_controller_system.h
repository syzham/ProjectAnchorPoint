#ifndef SANDBOX_SYSTEMS_PLAYER_CONTROLLER_SYSTEM_H
#define SANDBOX_SYSTEMS_PLAYER_CONTROLLER_SYSTEM_H

#include <anchorpoint/components/transform.h>
#include <anchorpoint/core/engine.h>

#include "components/player_controller.h"
#include "game/state_system.h"

namespace sandbox {

// Mouse-look + WASD movement. Registered to run only while Playing, so movement
// freezes automatically when the game is paused.
class PlayerControllerSystem : public StateSystem {
public:
    PlayerControllerSystem() : StateSystem({GameState::Playing}) {}

protected:
    void OnUpdateInState(ap::Engine& engine) override {
        auto& input = engine.GetInput();
        const float deltaTime = engine.GetTime().deltaTime;

        if (input.IsKeyDown(ap::Key::Escape)) {
            engine.Quit();
            return;
        }

        engine.GetWorld().Each<ap::Transform, PlayerController>(
            [&](ap::Entity, ap::Transform& transform, PlayerController& player) {
                ap::AddRotation(transform,
                                {input.GetMouseDeltaX() * player.sensitivity,
                                 -input.GetMouseDeltaY() * player.sensitivity,
                                 0},
                                true);

                if (input.IsKeyDown(ap::Key::W)) ap::MoveForwards(transform, player.speed * deltaTime);
                if (input.IsKeyDown(ap::Key::S)) ap::MoveForwards(transform, -player.speed * deltaTime);
                if (input.IsKeyDown(ap::Key::A)) ap::MoveRight(transform, -player.speed * deltaTime);
                if (input.IsKeyDown(ap::Key::D)) ap::MoveRight(transform, player.speed * deltaTime);
            });
    }
};

} // namespace sandbox

#endif //SANDBOX_SYSTEMS_PLAYER_CONTROLLER_SYSTEM_H
