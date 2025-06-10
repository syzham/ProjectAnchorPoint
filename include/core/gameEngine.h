#ifndef PROJECTANCHORPOINT_GAMEENGINE_H
#define PROJECTANCHORPOINT_GAMEENGINE_H

#include "graphics/graphics.h"
#include "game/SceneManager.h"
#include "game/ScriptBehaviour.h"

class GameEngine {
public:
    void Init();
    void Update();
    void Shutdown();
};

#endif //PROJECTANCHORPOINT_GAMEENGINE_H
