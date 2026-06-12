#pragma once

#include "core/System.h"
#include "game/components/transform.h"
#include "game/components/cameracomponent.h"
#include "graphics/camera.h"

class CameraSystem : public ISystem {
public:
    void Update(Registry& registry, float dt) override;
};
