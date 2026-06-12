#ifndef ANCHORPOINT_SYSTEMS_RENDER_SYSTEM_H
#define ANCHORPOINT_SYSTEMS_RENDER_SYSTEM_H

#include "anchorpoint/core/api.h"
#include "anchorpoint/ecs/system.h"

namespace ap {

// Gathers the active camera, lights and mesh renderers from the World into a
// FrameData and hands it to the renderer backend. Added automatically by
// Engine::Init() as the last system, so it draws the state produced by all
// gameplay systems this frame.
class AP_API RenderSystem : public System {
public:
    void OnUpdate(Engine& engine) override;
};

} // namespace ap

#endif //ANCHORPOINT_SYSTEMS_RENDER_SYSTEM_H
