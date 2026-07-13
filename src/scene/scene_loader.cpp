#include "anchorpoint/scene/scene_loader.h"

#include <fstream>

#include "anchorpoint/components/name.h"
#include "anchorpoint/core/log.h"

namespace ap {

void SceneLoader::RegisterComponent(const std::string& name, LoaderFn loader) {
    loaders[name] = std::move(loader);
}

bool SceneLoader::LoadScene(const std::string& name, World& world) {
    LogHeader("Loading Scene");

    std::ifstream in("scenes/" + name + ".scene");
    if (!in) {
        LogError(("Failed to open scene: " + name).c_str());
        return false;
    }

    json sceneData;
    in >> sceneData;

    for (const auto& obj : sceneData["objects"]) {
        const Entity entity = world.CreateEntity();
        for (const auto& component : obj.items()) {
            if (component.key() == "Name") {
                Log(component.value().get<std::string>().c_str());
                world.Add<Name>(entity, component.value().get<std::string>());
                continue;
            }
            Log(component.key().c_str(), 1);

            const auto it = loaders.find(component.key());
            if (it == loaders.end()) {
                LogError(("Unknown component type: " + component.key()).c_str());
                continue;
            }
            it->second(world, entity, component.value());
        }
    }

    return true;
}

void SceneLoader::UnloadScene(World& world) {
    world.Clear();
}

} // namespace ap
