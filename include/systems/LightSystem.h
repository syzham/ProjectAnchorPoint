#pragma once

#include "core/System.h"
#include "game/components/transform.h"
#include "game/components/lightcomponent.h"
#include "graphics/light.h"

class LightSystem : public ISystem {
public:
    void Update(Registry& registry, float dt) override;
};
