#ifndef PROJECTANCHORPOINT_WINDOW_H
#define PROJECTANCHORPOINT_WINDOW_H

#include <Windows.h>
#include "core/config.h"
#include "game/InputManager.h"

class Window {
public:
    int Create(HINSTANCE hInstance, int nCmdShow, const wchar_t* title);
    void Show();
    HWND getHwnd();
private:
    HWND hWnd;
    int cmdShow;
};

#endif //PROJECTANCHORPOINT_WINDOW_H
