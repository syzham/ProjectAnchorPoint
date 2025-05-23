#include "core/gameEngine.h"


void GameEngine::Init() {
    SceneManager::getInstance().setWindow(device, dc);
    SceneManager::getInstance().loadFirstScene();
}

void GameEngine::Update() {
    SceneManager::getInstance().update();
}

void GameEngine::Shutdown() {
    SceneManager::getInstance().unloadScene();
}