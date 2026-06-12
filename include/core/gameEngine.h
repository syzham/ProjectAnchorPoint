#pragma once

#include "graphics/graphics.h"
#include "game/SceneManager.h"
#include "game/ScriptBehaviour.h"
#include "window/window.h"
#include "window/cursor.h"
#include "core/Registry.h"
#include "core/System.h"
#include <memory>

class GameEngine {
public:
    int Init(HINSTANCE hInstance, int nCmdShow);
    void Run();
    void Shutdown();

    Registry& GetRegistry() { return SceneManager::getInstance().currentScene.registry; }

    void AddSystem(std::unique_ptr<ISystem> system) {
        SceneManager::getInstance().currentScene.AddSystem(std::move(system));
    }

private:
    Window window;
    Cursor cursor = nullptr;
};
