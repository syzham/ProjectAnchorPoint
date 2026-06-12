#pragma once
#include "core/Registry.h"

class ISystem {
public:
    virtual ~ISystem() = default;
    virtual void Update(Registry& registry, float dt) = 0;
};
