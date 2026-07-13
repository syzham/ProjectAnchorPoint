#ifndef ANCHORPOINT_SCENE_SCENE_LOADER_H
#define ANCHORPOINT_SCENE_SCENE_LOADER_H

#include <functional>
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include "anchorpoint/core/api.h"
#include "anchorpoint/ecs/world.h"

namespace ap {

// Data-driven scene loading. Each key inside a scene object maps to a
// registered component loader, which attaches that component to the entity.
// The engine registers loaders for the built-in components; projects register
// their own with RegisterComponent() before loading a scene.
class AP_API SceneLoader {
public:
    using json = nlohmann::ordered_json;
    using LoaderFn = std::function<void(World&, Entity, const json&)>;

    void RegisterComponent(const std::string& name, LoaderFn loader);

    // Loads scenes/<name>.scene into the world. Returns false if the file
    // is missing or unreadable.
    bool LoadScene(const std::string& name, World& world);

    void UnloadScene(World& world);

private:
    std::unordered_map<std::string, LoaderFn> loaders;
};

} // namespace ap

#endif //ANCHORPOINT_SCENE_SCENE_LOADER_H
