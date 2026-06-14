#ifndef ANCHORPOINT_PLATFORM_COCOA_WINDOW_H
#define ANCHORPOINT_PLATFORM_COCOA_WINDOW_H

#include <memory>

#include "anchorpoint/platform/window.h"

namespace ap {

// Cocoa (AppKit) window backend for macOS. All Objective-C state lives in the
// Impl defined in the .mm file so this header stays pure C++ and can be
// included from the plain-C++ platform factory.
class CocoaWindow final : public Window {
public:
    CocoaWindow();
    ~CocoaWindow() override;

    bool Create(const EngineConfig& config, Input& input) override;
    void Show() override;
    bool PumpEvents() override;
    void SetCursorCaptured(bool captured) override;
    void* GetNativeHandle() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace ap

#endif //ANCHORPOINT_PLATFORM_COCOA_WINDOW_H
