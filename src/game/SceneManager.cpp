#include "game/SceneManager.h"

void SceneManager::loadFirstScene() {
    if (sceneName == firstScene) {
        return;
    }

    if (!sceneName.empty()) {
        currentScene.Unload();
    }

    sceneName = firstScene;
    currentScene.Load(firstScene);
}

void SceneManager::update() {
    currentScene.Draw();
}

void SceneManager::unloadScene() {
    currentScene.Unload();
}