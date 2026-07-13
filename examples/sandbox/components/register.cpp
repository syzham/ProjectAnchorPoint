#include "components/register.h"

#include "components/player_controller.h"
#include "components/spin.h"

namespace sandbox {

using json = ap::SceneLoader::json;

void RegisterComponents(ap::SceneLoader& scenes) {
    scenes.RegisterComponent("PlayerController",
        [](ap::World& world, ap::Entity entity, const json& data) {
            auto& player = world.Add<PlayerController>(entity);
            if (data.contains("sensitivity")) player.sensitivity = data["sensitivity"].get<float>();
            if (data.contains("speed")) player.speed = data["speed"].get<float>();
        });

    scenes.RegisterComponent("Spin",
        [](ap::World& world, ap::Entity entity, const json& data) {
            auto& spin = world.Add<Spin>(entity);
            spin.delta = {data["delta"][0].get<float>(),
                          data["delta"][1].get<float>(),
                          data["delta"][2].get<float>()};
        });
}

} // namespace sandbox
