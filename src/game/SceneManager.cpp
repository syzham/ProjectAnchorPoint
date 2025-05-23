#include "game/SceneManager.h"

void SceneManager::loadFirstScene() {
    if (sceneName == firstScene) {
        return;
    }

    if (!sceneName.empty()) {
        currentScene.Unload();
    }

    sceneName = firstScene;
    currentScene.Load(firstScene, device);
}

void SceneManager::update() {
    currentScene.Draw(context);
}

void SceneManager::unloadScene() {
    currentScene.Unload();
}

void SceneManager::setWindow(ID3D11Device *dev, ID3D11DeviceContext *con) {
    device = dev;
    context = con;
}