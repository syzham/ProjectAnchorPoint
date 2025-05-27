#ifndef PROJECTANCHORPOINT_COMPONENT_H
#define PROJECTANCHORPOINT_COMPONENT_H

#include <memory>
#include <functional>
#include <string>
#include "graphics/scene.h"
#include "nlohmann/json.hpp"
using json = nlohmann::ordered_json;

#define REGISTER_COMPONENT(TYPE) \
    static bool _##TYPE##_registered = []() { \
    ComponentFactory::Instance().Register(#TYPE, []() { return std::make_unique<TYPE>(); }); \
    return true;                 \
    }()

class SceneObject;

class Component {
public:
    SceneObject *owner = nullptr;
    virtual ~Component() = default;
    virtual void Init(nlohmann::basic_json<> data) = 0;
    virtual void Update() = 0;
    virtual void Destroy() = 0;
    virtual std::string getName() = 0;

};

using ComponentCreateFunc = std::function<std::unique_ptr<Component>()>;
class ComponentFactory {
public:
    static ComponentFactory& Instance() {
        static ComponentFactory instance;
        return instance;
    }

    void Register(const std::string& name, ComponentCreateFunc func) {
        registry[name] = func;
    }

    std::unique_ptr<Component> Create(const std::string& name) const {
        auto it = registry.find(name);
        if (it != registry.end()) return it->second();
        return nullptr;
    }

private:
    std::unordered_map<std::string, ComponentCreateFunc> registry;
};

#endif //PROJECTANCHORPOINT_COMPONENT_H
