// Sandbox: an example game built on the AnchorPoint engine library.
//
// Gameplay is organised into folders so this file stays a readable description
// of the game rather than its implementation:
//
//   components/  plain-data components (+ their scene loaders)
//   systems/     behaviour, one System per file
//   game/        the game-state machine (GameState + StateSystem)
//
// Systems declare which game state they run in by deriving from StateSystem
// (see game/state_system.h); the active state is just a singleton component in
// the World, so state handling stays pure-ECS.

#include <cstring>
#include <string>

#include <anchorpoint/anchorpoint.h>

#include "components/register.h"
#include "game/game_state.h"
#include "systems/debug_toggle_system.h"
#include "systems/pause_system.h"
#include "systems/player_controller_system.h"
#include "systems/spin_system.h"

int main(int argc, char** argv) {
    ap::EngineConfig config;
    config.title = "Project Anchor Point";
    config.startScene = "first";

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--headless") == 0)
            config.headless = true;
        else if (std::strcmp(argv[i], "--debug-colliders") == 0)
            config.debugDrawColliders = true;
        else if (std::strcmp(argv[i], "--frames") == 0 && i + 1 < argc)
            config.maxFrames = std::stoull(argv[++i]);
    }

    ap::LogHeader("Project Anchor Point");

    ap::Engine engine(config);

    // Make the sandbox's custom components loadable from .scene files.
    sandbox::RegisterComponents(engine.GetScenes());

    // PlayerController and Spin run only while Playing; Pause and DebugToggle
    // run in every state. Systems added before Init() run before the engine's
    // built-in collision/render systems each frame.
    engine.AddSystem<sandbox::PlayerControllerSystem>();
    engine.AddSystem<sandbox::SpinSystem>();
    engine.AddSystem<sandbox::PauseSystem>();      // press P to pause/resume
    engine.AddSystem<sandbox::DebugToggleSystem>(); // press B for collider outlines

    if (engine.Init() != 0) {
        ap::LogError("Failed to initialize engine");
        return -1;
    }

    // Start in the Playing state (creates the singleton GameStateContext).
    sandbox::SetState(engine.GetWorld(), sandbox::GameState::Playing);

    engine.Run();
    return 0;
}
