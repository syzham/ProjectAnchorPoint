#include "game/components/scriptcomponent.h"
#include "game/components/transform.h"


std::string ScriptComponent::getName() {
    return "ScriptComponent";
}

void ScriptComponent::Init(nlohmann::basic_json<> data) {
    create_fn(data["Script"].get<std::string>().c_str());
    for (auto dat : data["data"]) {
        if (dat["type"] == "component") {
            auto comp = owner->components[dat["value"].get<int>()].get();
            setPointer_fn(dat["name"].get<std::string>().c_str(), (void*)comp);
        } else if (dat["type"] == "vector3") {
            float vec[3] = {dat["value"][0].get<float>(), dat["value"][1].get<float>(), dat["value"][2].get<float>()};
            setVector3_fn(dat["name"].get<std::string>().c_str(), vec);
        }
    }
}

void ScriptComponent::Update() {
}

void ScriptComponent::Destroy() {
}

