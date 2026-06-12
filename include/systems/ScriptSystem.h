#pragma once

#include "core/System.h"

class ScriptSystem : public ISystem {
public:
    void Update(Registry& registry, float dt) override;
};
