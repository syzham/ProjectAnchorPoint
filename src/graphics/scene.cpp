#include "graphics/scene.h"

void Scene::Load(const std::string& sceneFile) {

    LogHeader("Loading Scene");

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
                Log(component.value().get<std::string>().c_str());
                newObject->name = component.value();
                continue;
            }
            Log(component.key().c_str(), 1);

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
