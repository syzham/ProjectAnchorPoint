#include "core/gameEngine.h"


void GameEngine::Init() {
#ifdef _DEBUG
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif
    InitHost();
    SceneManager::getInstance().loadFirstScene();
}

void GameEngine::Update() {
    UpdateScript();
    SceneManager::getInstance().update();
}

void GameEngine::Shutdown() {
    CloseHost();
    SceneManager::getInstance().unloadScene();
}