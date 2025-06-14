#include "window/cursor.h"

Cursor::Cursor(HWND hwnd) {
    hWnd = hwnd;
}

void Cursor::Hide() {
    ShowCursor(false);

    RECT rect;
    GetClientRect(hWnd, &rect);
    POINT ul = { rect.left, rect.top };
    POINT lr = { rect.right, rect.bottom };
    ClientToScreen(hWnd, &ul);
    ClientToScreen(hWnd, &lr);
    rect.left = ul.x;
    rect.top = ul.y;
    rect.right = lr.x;
    rect.bottom = lr.y;
    ClipCursor(&rect);
}

void Cursor::Show() {
    ShowCursor(true);
    ClipCursor(nullptr);
}

void Cursor::Centre() {
    POINT center;
    center.x = screenWidth / 2;
    center.y = screenHeight / 2;
    ClientToScreen(hWnd, &center);
    SetCursorPos(center.x, center.y);
}