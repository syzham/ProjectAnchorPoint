#ifndef ANCHORPOINT_PLATFORM_INPUT_H
#define ANCHORPOINT_PLATFORM_INPUT_H

#include <cstddef>

#include "anchorpoint/platform/key.h"

namespace ap {

// Per-frame input state owned by the Engine. Window backends feed events in;
// systems read it through Engine::GetInput().
class Input {
public:
    bool IsKeyDown(Key key) const {
        return keys[static_cast<std::size_t>(key)];
    }

    float GetMouseDeltaX() const { return mouseDX; }
    float GetMouseDeltaY() const { return mouseDY; }

    // --- called by window backends ---

    void SetKeyDown(Key key, bool isDown) {
        keys[static_cast<std::size_t>(key)] = isDown;
    }

    void AddMouseDelta(float dx, float dy) {
        mouseDX += dx;
        mouseDY += dy;
    }

    // Called at the start of each event pump so deltas cover one frame.
    void BeginFrame() {
        mouseDX = 0;
        mouseDY = 0;
    }

private:
    bool keys[static_cast<std::size_t>(Key::Count)] = {false};
    float mouseDX = 0;
    float mouseDY = 0;
};

} // namespace ap

#endif //ANCHORPOINT_PLATFORM_INPUT_H
