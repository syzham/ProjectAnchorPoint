#include "core/gameEngine.h"


void GameEngine::Init() {
#ifdef _DEBUG
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif
    SceneManager::getInstance().loadFirstScene();
}

void GameEngine::Update() {
    SceneManager::getInstance().update();
}

void GameEngine::Shutdown() {
    SceneManager::getInstance().unloadScene();
}