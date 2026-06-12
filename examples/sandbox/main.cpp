// Sandbox: an example game built on the AnchorPoint engine library.
//
// Demonstrates the pure-ECS workflow: the game defines its own plain-data
// components (PlayerController, Spin), registers scene loaders for them, and
// implements behaviour in C++ systems.

#include <cstring>
#include <string>

#include <anchorpoint/anchorpoint.h>

using json = ap::SceneLoader::json;

// --- Custom components (plain data) ---

struct PlayerController {
    float sensitivity = 0.001f;
    float speed = 9.0f;
};

struct Spin {
    ap::Vector3 delta;
};

// --- Custom systems (behaviour) ---

class PlayerControllerSystem : public ap::System {
public:
    void OnUpdate(ap::Engine& engine) override {
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

                if (input.IsKeyDown(ap::Key::W))
                    ap::MoveForwards(transform, player.speed * deltaTime);
                if (input.IsKeyDown(ap::Key::S))
                    ap::MoveForwards(transform, -player.speed * deltaTime);
                if (input.IsKeyDown(ap::Key::A))
                    ap::MoveRight(transform, -player.speed * deltaTime);
                if (input.IsKeyDown(ap::Key::D))
                    ap::MoveRight(transform, player.speed * deltaTime);
            });
    }
};

class SpinSystem : public ap::System {
public:
    void OnUpdate(ap::Engine& engine) override {
        engine.GetWorld().Each<ap::Transform, Spin>(
            [](ap::Entity, ap::Transform& transform, Spin& spin) {
                ap::AddRotation(transform, spin.delta);
            });
    }
};

int main(int argc, char** argv) {
    ap::EngineConfig config;
    config.title = "Project Anchor Point";
    config.startScene = "first";

    // --headless --frames N lets the sandbox run without a window/GPU,
    // e.g. on CI or non-Windows platforms without a renderer backend.
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--headless") == 0)
            config.headless = true;
        else if (std::strcmp(argv[i], "--frames") == 0 && i + 1 < argc)
            config.maxFrames = std::stoull(argv[++i]);
    }

    ap::LogHeader("Project Anchor Point");

    ap::Engine engine(config);

    // Make the custom components loadable from .scene files.
    engine.GetScenes().RegisterComponent("PlayerController",
        [](ap::World& world, ap::Entity entity, const json& data) {
            auto& player = world.Add<PlayerController>(entity);
            if (data.contains("sensitivity")) player.sensitivity = data["sensitivity"].get<float>();
            if (data.contains("speed")) player.speed = data["speed"].get<float>();
        });

    engine.GetScenes().RegisterComponent("Spin",
        [](ap::World& world, ap::Entity entity, const json& data) {
            auto& spin = world.Add<Spin>(entity);
            spin.delta = {data["delta"][0].get<float>(),
                          data["delta"][1].get<float>(),
                          data["delta"][2].get<float>()};
        });

    // Gameplay systems run before the engine's built-in collision/render
    // systems because they are added before Init().
    engine.AddSystem<PlayerControllerSystem>();
    engine.AddSystem<SpinSystem>();

    if (engine.Init() != 0) {
        ap::LogError("Failed to initialize engine");
        return -1;
    }

    engine.Run();
    return 0;
}
