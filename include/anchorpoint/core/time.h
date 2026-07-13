#ifndef ANCHORPOINT_CORE_TIME_H
#define ANCHORPOINT_CORE_TIME_H

#include <cstdint>

namespace ap {

struct Time {
    float deltaTime = 0.0f;     // seconds since the previous frame (clamped)
    float fixedDelta = 1.0f / 60.0f;
    double totalTime = 0.0;     // seconds since Engine::Run() started
    std::uint64_t frameCount = 0;
};

} // namespace ap

#endif //ANCHORPOINT_CORE_TIME_H
