#ifndef SANDBOX_GAME_STATE_SYSTEM_H
#define SANDBOX_GAME_STATE_SYSTEM_H

#include <algorithm>
#include <initializer_list>
#include <vector>

#include <anchorpoint/core/engine.h>
#include <anchorpoint/ecs/system.h>

#include "game/game_state.h"

namespace sandbox {

// Base class for systems that should only run in certain game states. A system
// "registers" the states it runs in by passing them to the constructor; an
// empty list means "run in every state".
//
// This keeps state handling pure-ECS: the active state is data in the World
// (a GameStateContext component), and the gate is just a query — no scheduler
// changes or special engine support required. Subclasses override the
// *_InState hooks instead of OnUpdate/OnFixedUpdate.
class StateSystem : public ap::System {
public:
    StateSystem(std::initializer_list<GameState> states) : activeStates(states) {}

    void OnUpdate(ap::Engine& engine) final {
        if (RunsIn(CurrentState(engine.GetWorld())))
            OnUpdateInState(engine);
    }

    void OnFixedUpdate(ap::Engine& engine) final {
        if (RunsIn(CurrentState(engine.GetWorld())))
            OnFixedUpdateInState(engine);
    }

protected:
    virtual void OnUpdateInState(ap::Engine& engine) {}
    virtual void OnFixedUpdateInState(ap::Engine& engine) {}

private:
    bool RunsIn(GameState state) const {
        return activeStates.empty()
            || std::find(activeStates.begin(), activeStates.end(), state) != activeStates.end();
    }

    std::vector<GameState> activeStates;
};

} // namespace sandbox

#endif //SANDBOX_GAME_STATE_SYSTEM_H
