#include "window/window.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_SETFOCUS:
            return 0;
        case WM_MOUSEMOVE: {
            InputManager::getInstance().MouseEvent(hwnd);

            return 0;
        }
        case WM_KEYDOWN: {
            InputManager::getInstance().setKeyDown(wParam, true);;
            return 0;
        }
        case WM_KEYUP: {
            InputManager::getInstance().setKeyDown(wParam, false);;
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

HWND Window::getHwnd() {
    return hWnd;
}

int Window::Create(HINSTANCE hInstance, int nCmdShow, const wchar_t* title) {
    const wchar_t CLASS_NAME[] = L"MainWindow";

    WNDCLASS wc = {};
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);

    RegisterClass(&wc);

    hWnd = CreateWindowEx(
            0,
            CLASS_NAME,
            title,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, screenWidth, screenHeight,
            nullptr, nullptr, hInstance, nullptr
    );
    cmdShow = nCmdShow;

    if (hWnd == nullptr)
        return -1;

    return 0;
}

void Window::Show() {
    ShowWindow(hWnd, cmdShow);
}