#pragma once

#include "graphics/scene.h"
#include "core/config.h"
#include <string>

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

    Scene currentScene;
    std::string sceneName;

    void loadFirstScene();
    void update(float dt);
    void unloadScene();

private:
    SceneManager() = default;
    void addDefaultSystems();
};
