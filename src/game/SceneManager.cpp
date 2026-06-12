#include "game/SceneManager.h"
#include "systems/RenderSystem.h"
#include "systems/CameraSystem.h"
#include "systems/LightSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/ScriptSystem.h"

void SceneManager::addDefaultSystems() {
    currentScene.AddSystem(std::make_unique<ScriptSystem>());
    currentScene.AddSystem(std::make_unique<CameraSystem>());
    currentScene.AddSystem(std::make_unique<LightSystem>());
    currentScene.AddSystem(std::make_unique<CollisionSystem>());
    currentScene.AddSystem(std::make_unique<RenderSystem>());
}

void SceneManager::loadFirstScene() {
    if (sceneName == firstScene) return;

    if (!sceneName.empty())
        currentScene.Unload();

    sceneName = firstScene;
    addDefaultSystems();
    currentScene.Load(firstScene);
}

void SceneManager::update(float dt) {
    currentScene.Update(dt);
}

void SceneManager::unloadScene() {
    currentScene.Unload();
}
