#ifndef PROJECTANCHORPOINT_WINDOW_H
#define PROJECTANCHORPOINT_WINDOW_H

#include <windows.h>
#include "core/config.h"

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
