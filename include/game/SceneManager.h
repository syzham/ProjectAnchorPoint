#ifndef PROJECTANCHORPOINT_SCENEMANAGER_H
#define PROJECTANCHORPOINT_SCENEMANAGER_H

#include "graphics/scene.h"
#include "core/config.h"

class SceneManager {
public:
    static SceneManager& getInstance() {
        static SceneManager instance;
        return instance;
    }

    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
    SceneManager(SceneManager&&) = delete;
    SceneManager& operator=(const SceneManager&&) = delete;

    Scene currentScene = {};
    std::string sceneName = "";
    void loadFirstScene();
    void update();
    void unloadScene();
private:
    SceneManager() = default;
};

#endif //PROJECTANCHORPOINT_SCENEMANAGER_H
