#ifndef ANCHORPOINT_CORE_CONFIG_H
#define ANCHORPOINT_CORE_CONFIG_H

#include <cstdint>
#include <string>

namespace ap {

struct EngineConfig {
    std::string title = "AnchorPoint";
    int width = 800;
    int height = 600;

    float fixedDelta = 1.0f / 60.0f;
    float clearColor[4] = {0.2f, 0.4f, 0.6f, 1.0f};

    // Centre and hide the cursor for FPS-style mouse-look input.
    bool captureCursor = true;

    // Run without a window or GPU (servers, tests, CI). Uses the null
    // platform backend on every OS.
    bool headless = false;

    // Stop the main loop after this many frames; 0 means run until quit.
    std::uint64_t maxFrames = 0;

    // Scene (from scenes/<name>.scene) loaded automatically during Init().
    std::string startScene;
};

} // namespace ap

#endif //ANCHORPOINT_CORE_CONFIG_H
