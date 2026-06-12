#pragma once

#include "core/System.h"
#include "game/components/transform.h"
#include "game/components/meshrenderer.h"

class RenderSystem : public ISystem {
public:
    void Update(Registry& registry, float dt) override;
};
