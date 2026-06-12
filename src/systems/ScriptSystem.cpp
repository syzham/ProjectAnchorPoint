#include "systems/ScriptSystem.h"
#include "game/ScriptBehaviour.h"

void ScriptSystem::Update(Registry& registry, float dt) {
    UpdateScript(dt);
}
