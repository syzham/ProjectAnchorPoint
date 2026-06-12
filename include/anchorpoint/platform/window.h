#ifndef ANCHORPOINT_PLATFORM_WINDOW_H
#define ANCHORPOINT_PLATFORM_WINDOW_H

#include <memory>

#include "anchorpoint/core/api.h"
#include "anchorpoint/core/config.h"
#include "anchorpoint/platform/input.h"

namespace ap {

// Abstract OS window. Backends translate native events into Input state.
class Window {
public:
    virtual ~Window() = default;

    virtual bool Create(const EngineConfig& config, Input& input) = 0;
    virtual void Show() = 0;

    // Pumps pending OS events; returns false when the window was closed.
    virtual bool PumpEvents() = 0;

    virtual void SetCursorCaptured(bool captured) = 0;

    // Native handle (HWND on Windows), for renderer backends.
    virtual void* GetNativeHandle() const = 0;
};

// Creates the window backend for the current platform (or the null backend
// when config.headless is set or no native backend exists for the platform).
AP_API std::unique_ptr<Window> CreatePlatformWindow(const EngineConfig& config);

} // namespace ap

#endif //ANCHORPOINT_PLATFORM_WINDOW_H
