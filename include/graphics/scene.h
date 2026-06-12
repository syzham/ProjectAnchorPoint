#pragma once

#include <vector>
#include <memory>
#include <string>
#include "graphics/camera.h"
#include "core/Registry.h"
#include "core/System.h"

class Scene {
public:
    Registry registry;
    Camera camera;
    std::vector<std::unique_ptr<ISystem>> systems;

    void AddSystem(std::unique_ptr<ISystem> system);
    void Load(const std::string& sceneFile);
    void Update(float dt);
    void Unload();
};
