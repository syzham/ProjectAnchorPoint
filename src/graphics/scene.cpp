#include "graphics/scene.h"
#include "game/components/transform.h"
#include "game/components/meshrenderer.h"
#include "game/components/lightcomponent.h"
#include "game/components/cameracomponent.h"
#include "game/components/colliders/aacollider.h"
#include "game/components/scriptcomponent.h"
#include "game/ScriptBehaviour.h"
#include "game/InputManager.h"
#include "graphics/light.h"
#include "core/logger.h"
#include "nlohmann/json.hpp"
#include <fstream>

using json = nlohmann::ordered_json;

void Scene::AddSystem(std::unique_ptr<ISystem> system) {
    systems.push_back(std::move(system));
}

void Scene::Load(const std::string& sceneFile) {
    LogHeader("Loading Scene");

    std::ifstream in("scenes/" + sceneFile + ".scene");
    if (!in) return;

    json sceneData;
    in >> sceneData;

    for (const auto& obj : sceneData["objects"]) {
        Entity e = registry.create();

        if (obj.contains("Name"))
            Log(obj["Name"].get<std::string>().c_str());

        if (obj.contains("Transform")) {
            const auto& d = obj["Transform"];
            Transform t;
            t.position = {d["position"][0], d["position"][1], d["position"][2]};
            t.rotation = {d["rotation"][0], d["rotation"][1], d["rotation"][2]};
            t.scale    = {d["scale"][0],    d["scale"][1],    d["scale"][2]};
            registry.emplace<Transform>(e, t);
        }

        if (obj.contains("MeshRenderer")) {
            const auto& d = obj["MeshRenderer"];
            registry.emplace<MeshRenderer>(e);
            MeshRenderer& mr = registry.get<MeshRenderer>(e);
            mr.topology = MeshRenderer::ParseTopology(d["topology"]);
            mr.mesh.LoadFromOBJ(d["model"], mr.topology);
            mr.mesh.CreateVertexBuffer();
            mr.initialized = true;
        }

        if (obj.contains("LightComponent")) {
            const auto& d = obj["LightComponent"];
            LightComp lc;
            lc.light.type      = d["lightType"];
            lc.light.range     = d["range"];
            lc.light.intensity = d["intensity"];
            lc.light.color.x   = d["color"][0];
            lc.light.color.y   = d["color"][1];
            lc.light.color.z   = d["color"][2];
            if (registry.has<Transform>(e)) {
                lc.light.position  = registry.get<Transform>(e).position;
                lc.light.direction = registry.get<Transform>(e).rotation;
            }
            registry.emplace<LightComp>(e, lc);
        }

        if (obj.contains("CameraComponent")) {
            CameraComp cc;
            cc.isActive = obj["CameraComponent"].contains("active");
            registry.emplace<CameraComp>(e, cc);
        }

        if (obj.contains("AACollider")) {
            const auto& d = obj["AACollider"];
            AABBCollider col;
            col.offset   = {d["offset"][0], d["offset"][1], d["offset"][2]};
            col.size     = {d["size"][0],   d["size"][1],   d["size"][2]};
            col.isStatic = d["static"];
            registry.emplace<AABBCollider>(e, col);
        }

        if (obj.contains("ScriptComponent")) {
            const auto& d = obj["ScriptComponent"];
            ScriptComp sc;
            sc.scriptClass = d["Script"];
            registry.emplace<ScriptComp>(e, sc);

            create_fn(sc.scriptClass.c_str());

            if (d.contains("data")) {
                for (const auto& dat : d["data"]) {
                    const std::string name = dat["name"];
                    const std::string type = dat["type"];

                    if (type == "pointer") {
                        const std::string compType = dat["componentType"];
                        if (compType == "Transform" && registry.has<Transform>(e))
                            setPointer_fn(name.c_str(), &registry.get<Transform>(e));
                        else if (compType == "InputManager")
                            setPointer_fn(name.c_str(), &InputManager::getInstance());
                    } else if (type == "float") {
                        setFloat_fn(name.c_str(), dat["value"].get<float>());
                    } else if (type == "vector3") {
                        float vec[3] = {dat["value"][0], dat["value"][1], dat["value"][2]};
                        setVector3_fn(name.c_str(), vec);
                    }
                }
            }
        }
    }

    Lights::Init();
}

void Scene::Update(float dt) {
    for (auto& system : systems)
        system->Update(registry, dt);
}

void Scene::Unload() {
    registry.clear();
    systems.clear();
    Lights::Release();
}
