#include "anchorpoint/core/engine.h"

#include <chrono>

#include "anchorpoint/components/camera.h"
#include "anchorpoint/components/collider.h"
#include "anchorpoint/components/light.h"
#include "anchorpoint/components/mesh_renderer.h"
#include "anchorpoint/components/transform.h"
#include "anchorpoint/core/log.h"
#include "anchorpoint/systems/collision_system.h"
#include "anchorpoint/systems/render_system.h"

namespace ap {

namespace {

Vector3 ReadVector3(const SceneLoader::json& data) {
    return {data[0].get<float>(), data[1].get<float>(), data[2].get<float>()};
}

} // namespace

Engine::Engine(EngineConfig engineConfig) : config(std::move(engineConfig)) {
    time.fixedDelta = config.fixedDelta;
    debugDrawColliders = config.debugDrawColliders;
}

Engine::~Engine() {
    if (initialized) Shutdown();
}

void Engine::RegisterBuiltinComponents() {
    sceneLoader.RegisterComponent("Transform", [](World& world, Entity entity, const SceneLoader::json& data) {
        auto& transform = world.Add<Transform>(entity);
        transform.position = ReadVector3(data["position"]);
        transform.rotation = ReadVector3(data["rotation"]);
        transform.scale = ReadVector3(data["scale"]);
    });

    sceneLoader.RegisterComponent("MeshRenderer", [](World& world, Entity entity, const SceneLoader::json& data) {
        auto& meshRenderer = world.Add<MeshRenderer>(entity);
        meshRenderer.model = data["model"].get<std::string>();
        if (data.contains("topology"))
            meshRenderer.topology = ParseTopology(data["topology"].get<std::string>());
    });

    sceneLoader.RegisterComponent("Camera", [](World& world, Entity entity, const SceneLoader::json& data) {
        auto& camera = world.Add<Camera>(entity);
        camera.active = data.contains("active") && data["active"].get<int>() != 0;
        if (data.contains("fov")) camera.fov = data["fov"].get<float>();
        if (data.contains("near")) camera.nearZ = data["near"].get<float>();
        if (data.contains("far")) camera.farZ = data["far"].get<float>();
    });

    sceneLoader.RegisterComponent("Light", [](World& world, Entity entity, const SceneLoader::json& data) {
        auto& light = world.Add<Light>(entity);
        light.type = static_cast<LightType>(data["lightType"].get<int>());
        light.range = data["range"].get<float>();
        light.intensity = data["intensity"].get<float>();
        light.color = ReadVector3(data["color"]);
    });

    sceneLoader.RegisterComponent("AABBCollider", [](World& world, Entity entity, const SceneLoader::json& data) {
        auto& collider = world.Add<AABBCollider>(entity);
        collider.offset = ReadVector3(data["offset"]);
        collider.size = ReadVector3(data["size"]);
        collider.isStatic = data["static"].get<bool>();
    });
}

int Engine::Init() {
    Log("Initializing engine...");

    window = CreatePlatformWindow(config);
    if (!window || !window->Create(config, input)) {
        LogError("Failed to create window");
        return -1;
    }

    renderer = CreatePlatformRenderer(config);
    if (!renderer || !renderer->Init(*window, config)) {
        LogError("Failed to initialize renderer");
        return -1;
    }

    RegisterBuiltinComponents();

    AddSystem<CollisionSystem>();
    AddSystem<RenderSystem>();

    initialized = true;
    for (auto& system : systems)
        system->OnInit(*this);

    if (!config.startScene.empty())
        LoadScene(config.startScene);

    return 0;
}

void Engine::LoadScene(const std::string& name) {
    sceneLoader.LoadScene(name, world);
}

void Engine::UnloadScene() {
    sceneLoader.UnloadScene(world);
}

void Engine::Quit() {
    quitRequested = true;
}

void Engine::Run() {
    if (!initialized) {
        LogError("Engine::Run() called before Init()");
        return;
    }

    window->Show();
    window->SetCursorCaptured(config.captureCursor);

    using clock = std::chrono::steady_clock;
    auto lastFrame = clock::now();
    double accumulator = 0.0;

    while (!quitRequested) {
        input.BeginFrame();
        if (!window->PumpEvents())
            break;

        const auto currentFrame = clock::now();
        double deltaTime = std::chrono::duration<double>(currentFrame - lastFrame).count();
        lastFrame = currentFrame;
        if (deltaTime > 0.25) deltaTime = 0.25;

        time.deltaTime = static_cast<float>(deltaTime);
        time.totalTime += deltaTime;
        ++time.frameCount;

        accumulator += deltaTime;
        while (accumulator >= time.fixedDelta) {
            for (auto& system : systems)
                system->OnFixedUpdate(*this);
            accumulator -= time.fixedDelta;
        }

        for (auto& system : systems)
            system->OnUpdate(*this);

        if (config.maxFrames != 0 && time.frameCount >= config.maxFrames)
            break;
    }

    Shutdown();
}

void Engine::Shutdown() {
    if (!initialized) return;
    initialized = false;

    for (auto it = systems.rbegin(); it != systems.rend(); ++it)
        (*it)->OnShutdown(*this);

    UnloadScene();

    if (renderer) renderer->Shutdown();
    renderer.reset();
    window.reset();
}

} // namespace ap
