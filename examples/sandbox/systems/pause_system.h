#ifndef SANDBOX_SYSTEMS_PAUSE_SYSTEM_H
#define SANDBOX_SYSTEMS_PAUSE_SYSTEM_H

#include <anchorpoint/core/engine.h>
#include <anchorpoint/core/log.h>
#include <anchorpoint/ecs/system.h>

#include "game/game_state.h"

namespace sandbox {

// Toggles between Playing and Paused when P is pressed. This is a plain
// ap::System (not a StateSystem) because it must run in *every* state — it is
// what gets the game out of the Paused state. Edge-detected so a held key
// flips the state once.
class PauseSystem : public ap::System {
public:
    void OnUpdate(ap::Engine& engine) override {
        const bool down = engine.GetInput().IsKeyDown(ap::Key::P);
        if (down && !wasDown) {
            auto& world = engine.GetWorld();
            const GameState next = (CurrentState(world) == GameState::Playing)
                                 ? GameState::Paused
                                 : GameState::Playing;
            SetState(world, next);
            ap::Log(next == GameState::Paused ? "Paused" : "Playing");
        }
        wasDown = down;
    }

private:
    bool wasDown = false;
};

} // namespace sandbox

#endif //SANDBOX_SYSTEMS_PAUSE_SYSTEM_H
