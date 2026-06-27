#ifndef SANDBOX_SYSTEMS_DEBUG_TOGGLE_SYSTEM_H
#define SANDBOX_SYSTEMS_DEBUG_TOGGLE_SYSTEM_H

#include <anchorpoint/core/engine.h>
#include <anchorpoint/ecs/system.h>

namespace sandbox {

// Toggles the collider wireframe overlay when B is pressed. Runs in every state
// (a plain ap::System), so colliders can be inspected while paused too.
// Edge-detected so a held key flips it once rather than every frame.
class DebugToggleSystem : public ap::System {
public:
    void OnUpdate(ap::Engine& engine) override {
        const bool down = engine.GetInput().IsKeyDown(ap::Key::B);
        if (down && !wasDown)
            engine.SetDebugDrawColliders(!engine.IsDebugDrawColliders());
        wasDown = down;
    }

private:
    bool wasDown = false;
};

} // namespace sandbox

#endif //SANDBOX_SYSTEMS_DEBUG_TOGGLE_SYSTEM_H
