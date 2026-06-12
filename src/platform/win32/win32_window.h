#ifndef ANCHORPOINT_PLATFORM_WIN32_WINDOW_H
#define ANCHORPOINT_PLATFORM_WIN32_WINDOW_H

#include <Windows.h>

#include "anchorpoint/platform/window.h"

namespace ap {

class Win32Window final : public Window {
public:
    bool Create(const EngineConfig& config, Input& input) override;
    void Show() override;
    bool PumpEvents() override;
    void SetCursorCaptured(bool captured) override;
    void* GetNativeHandle() const override { return hWnd; }

    // Internal, called from the window procedure.
    void HandleMouseMove();
    void HandleKey(WPARAM virtualKey, bool isDown);

private:
    void CentreCursor() const;

    HWND hWnd = nullptr;
    Input* input = nullptr;
    int width = 0;
    int height = 0;
    bool cursorCaptured = false;
};

} // namespace ap

#endif //ANCHORPOINT_PLATFORM_WIN32_WINDOW_H
