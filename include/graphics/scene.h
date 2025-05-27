#ifndef PROJECTANCHORPOINT_SCENE_H
#define PROJECTANCHORPOINT_SCENE_H

#include <vector>
#include "graphics/graphics.h"
#include "graphics/camera.h"
#include "graphics/light.h"
#include "game/components/component.h"

class Component;
class SceneObject {
public:
    std::string name;
    std::vector<std::unique_ptr<Component>> components;

    template<typename T>
    T* getComponent(int instance) {
        for (auto& comp : components) {
            if (!comp || !comp.get()) continue;
            if (auto ret = dynamic_cast<T*>(comp.get())) {
                return ret;
            }
        }
        return nullptr;
    }

};

class Scene {
public:
    std::vector<std::unique_ptr<SceneObject>> objects;
    std::vector<Light> lights;
    ID3D11Buffer* lightBuffer = nullptr;
    ID3D11ShaderResourceView* lightSRV = nullptr;
    Camera camera;

    void Load(const std::string& sceneFile);
    void Draw();
    void Unload();
};

#endif //PROJECTANCHORPOINT_SCENE_H
