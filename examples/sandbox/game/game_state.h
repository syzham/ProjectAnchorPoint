#ifndef SANDBOX_GAME_GAME_STATE_H
#define SANDBOX_GAME_GAME_STATE_H

#include <anchorpoint/ecs/world.h>

namespace sandbox {

// The high-level states the game can be in. This is pure data: the *current*
// value lives in a singleton GameStateContext component inside the World, and
// systems gate themselves on it (see StateSystem). Nothing here is a global or
// a manager object — the state is just another component.
enum class GameState {
    Playing,
    Paused,
};

// Singleton component holding the active game state. Exactly one entity carries
// it; SetState creates that entity on first use.
struct GameStateContext {
    GameState current = GameState::Playing;
};

// Current state, defaulting to Playing if it has not been set yet.
GameState CurrentState(ap::World& world);

// Sets the current state, creating the singleton component if needed.
void SetState(ap::World& world, GameState state);

} // namespace sandbox

#endif //SANDBOX_GAME_GAME_STATE_H
