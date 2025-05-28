#include "graphics/scene.h"
#include <iostream>

void Scene::Load(const std::string& sceneFile) {

    std::ifstream in("scenes/" + sceneFile + ".scene");
    if (!in)
        return;

    json sceneData;
    in >> sceneData;

    for (const auto& obj : sceneData["objects"]) {
        objects.push_back(std::make_unique<SceneObject>());
        SceneObject* newObject = objects.back().get();
        for (const auto& component : obj.items()) {
            if (component.key() == "Name") {
                newObject->name = component.value();
                continue;
            }

            std::unique_ptr<Component> newComponent = ComponentFactory::Instance().Create(component.key());
            if (newComponent) {
                newComponent->owner = newObject;
                newComponent->Init(component.value());
                newObject->components.push_back(std::move(newComponent));
            }
        }
    }


    Lights::Init();
}

void Scene::Draw() {
    for (auto& object : objects) {
        for (auto& comp : object->components) {
            comp->Update();
        }
    }

    Lights::Update();
}

void Scene::Unload() {
    for (auto& object : objects) {
        for (auto& comp : object->components) {
            comp->Destroy();
        }
    }
    Lights::Release();
}
