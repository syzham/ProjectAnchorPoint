#ifndef PROJECTANCHORPOINT_CURSOR_H
#define PROJECTANCHORPOINT_CURSOR_H
#include "window/window.h"

class Cursor {
public:
    HWND hWnd;
    Cursor(HWND hwnd);
    void Hide();
    void Show();
    void Centre();
};

#endif //PROJECTANCHORPOINT_CURSOR_H
