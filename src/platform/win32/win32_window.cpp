#include "win32_window.h"

#include "anchorpoint/core/log.h"

namespace ap {

namespace {

Key TranslateKey(WPARAM virtualKey) {
    if (virtualKey >= 'A' && virtualKey <= 'Z')
        return static_cast<Key>(static_cast<int>(Key::A) + (virtualKey - 'A'));
    if (virtualKey >= '0' && virtualKey <= '9')
        return static_cast<Key>(static_cast<int>(Key::Num0) + (virtualKey - '0'));

    switch (virtualKey) {
        case VK_ESCAPE:  return Key::Escape;
        case VK_SPACE:   return Key::Space;
        case VK_RETURN:  return Key::Enter;
        case VK_TAB:     return Key::Tab;
        case VK_SHIFT:   return Key::Shift;
        case VK_CONTROL: return Key::Control;
        case VK_MENU:    return Key::Alt;
        case VK_LEFT:    return Key::Left;
        case VK_RIGHT:   return Key::Right;
        case VK_UP:      return Key::Up;
        case VK_DOWN:    return Key::Down;
        default:         return Key::Unknown;
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    auto* window = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (uMsg) {
        case WM_CREATE: {
            const auto* create = reinterpret_cast<CREATESTRUCT*>(lParam);
            SetWindowLongPtr(hwnd, GWLP_USERDATA,
                             reinterpret_cast<LONG_PTR>(create->lpCreateParams));
            return 0;
        }
        case WM_MOUSEMOVE:
            if (window) window->HandleMouseMove();
            return 0;
        case WM_KEYDOWN:
            if (window) window->HandleKey(wParam, true);
            return 0;
        case WM_KEYUP:
            if (window) window->HandleKey(wParam, false);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

} // namespace

bool Win32Window::Create(const EngineConfig& config, Input& inputState) {
    input = &inputState;
    width = config.width;
    height = config.height;

    const wchar_t CLASS_NAME[] = L"AnchorPointWindow";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClass(&wc);

    const std::wstring title(config.title.begin(), config.title.end());

    hWnd = CreateWindowEx(
            0,
            CLASS_NAME,
            title.c_str(),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, width, height,
            nullptr, nullptr, wc.hInstance, this
    );

    if (hWnd == nullptr) {
        LogError("Failed to create Win32 window");
        return false;
    }

    return true;
}

void Win32Window::Show() {
    ShowWindow(hWnd, SW_SHOW);
}

bool Win32Window::PumpEvents() {
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT)
            return false;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}

void Win32Window::SetCursorCaptured(bool captured) {
    if (captured == cursorCaptured) return;
    cursorCaptured = captured;
    if (captured) {
        CentreCursor();
        ShowCursor(FALSE);
    } else {
        ShowCursor(TRUE);
    }
}

void Win32Window::CentreCursor() const {
    POINT centre = {width / 2, height / 2};
    ClientToScreen(hWnd, &centre);
    SetCursorPos(centre.x, centre.y);
}

void Win32Window::HandleMouseMove() {
    if (!input || !cursorCaptured) return;

    POINT pos;
    GetCursorPos(&pos);

    POINT centre = {width / 2, height / 2};
    ClientToScreen(hWnd, &centre);

    const float dx = static_cast<float>(pos.x - centre.x);
    const float dy = static_cast<float>(pos.y - centre.y);

    if (dx != 0 || dy != 0) {
        input->AddMouseDelta(dx, dy);
        SetCursorPos(centre.x, centre.y);
    }
}

void Win32Window::HandleKey(WPARAM virtualKey, bool isDown) {
    if (!input) return;
    const Key key = TranslateKey(virtualKey);
    if (key != Key::Unknown)
        input->SetKeyDown(key, isDown);
}

} // namespace ap
