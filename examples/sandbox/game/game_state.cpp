#include "game/game_state.h"

namespace sandbox {

GameState CurrentState(ap::World& world) {
    GameState state = GameState::Playing;
    world.Each<GameStateContext>([&](ap::Entity, GameStateContext& ctx) {
        state = ctx.current;
    });
    return state;
}

void SetState(ap::World& world, GameState state) {
    bool found = false;
    world.Each<GameStateContext>([&](ap::Entity, GameStateContext& ctx) {
        ctx.current = state;
        found = true;
    });
    if (!found) {
        const ap::Entity entity = world.CreateEntity();
        world.Add<GameStateContext>(entity, GameStateContext{state});
    }
}

} // namespace sandbox
