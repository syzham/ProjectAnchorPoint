#ifndef PROJECTANCHORPOINT_GAMEENGINE_H
#define PROJECTANCHORPOINT_GAMEENGINE_H

#include "graphics/graphics.h"
#include "game/SceneManager.h"
#include "game/ScriptBehaviour.h"
#include "window/window.h"

class GameEngine {
public:
    int Init(HINSTANCE hInstance, int nCmdShow);
    void Run();
    void Shutdown();
private:
    Window window;
};

#endif //PROJECTANCHORPOINT_GAMEENGINE_H
