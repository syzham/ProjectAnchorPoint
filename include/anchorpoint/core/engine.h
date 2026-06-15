#ifndef ANCHORPOINT_CORE_ENGINE_H
#define ANCHORPOINT_CORE_ENGINE_H

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "anchorpoint/core/api.h"
#include "anchorpoint/core/config.h"
#include "anchorpoint/core/time.h"
#include "anchorpoint/ecs/system.h"
#include "anchorpoint/ecs/world.h"
#include "anchorpoint/platform/input.h"
#include "anchorpoint/platform/renderer.h"
#include "anchorpoint/platform/window.h"
#include "anchorpoint/scene/scene_loader.h"

namespace ap {

// Owns the World, the platform window/renderer, input state and the system
// schedule. Typical usage:
//
//     ap::EngineConfig config;
//     config.startScene = "first";
//     ap::Engine engine(config);
//     engine.GetScenes().RegisterComponent("MyComponent", ...);
//     engine.AddSystem<MyGameplaySystem>();
//     if (engine.Init() != 0) return -1;
//     engine.Run();
//
class AP_API Engine {
public:
    explicit Engine(EngineConfig config = {});
    ~Engine();

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    // Creates the window and renderer, registers the built-in component
    // loaders, appends the built-in collision/render systems and loads the
    // start scene. Returns 0 on success.
    int Init();

    // Runs the main loop until Quit(), the window closes, or
    // config.maxFrames is reached. Calls Shutdown() before returning.
    void Run();

    // Requests the main loop to stop after the current frame.
    void Quit();

    template<typename T, typename... Args>
    T& AddSystem(Args&&... args) {
        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *system;
        systems.push_back(std::move(system));
        if (initialized) ref.OnInit(*this);
        return ref;
    }

    void LoadScene(const std::string& name);
    void UnloadScene();

    // Wireframe collider overlay; initialised from EngineConfig and
    // toggleable at runtime (e.g. bound to a key in a debug build).
    void SetDebugDrawColliders(bool enabled) { debugDrawColliders = enabled; }
    bool IsDebugDrawColliders() const { return debugDrawColliders; }

    World& GetWorld() { return world; }
    Input& GetInput() { return input; }
    const Time& GetTime() const { return time; }
    SceneLoader& GetScenes() { return sceneLoader; }
    Renderer& GetRenderer() { return *renderer; }
    const EngineConfig& GetConfig() const { return config; }

private:
    void Shutdown();
    void RegisterBuiltinComponents();

    EngineConfig config;
    World world;
    Input input;
    Time time;
    SceneLoader sceneLoader;

    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    std::vector<std::unique_ptr<System>> systems;

    bool initialized = false;
    bool quitRequested = false;
    bool debugDrawColliders = false;
};

} // namespace ap

#endif //ANCHORPOINT_CORE_ENGINE_H
