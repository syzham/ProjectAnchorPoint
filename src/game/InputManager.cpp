#include "game/InputManager.h"

void InputManager::MouseEvent(HWND hwnd) {
    static POINT last = { screenWidth / 2, screenHeight / 2 };

    POINT pos;
    GetCursorPos(&pos);

    POINT clientCenter = { screenWidth / 2, screenHeight / 2 };

    ClientToScreen(hwnd, &clientCenter);

    dx = pos.x - clientCenter.x;
    dy = pos.y - clientCenter.y;

    if (dx != 0 || dy != 0) {
        SetCursorPos(clientCenter.x, clientCenter.y);
    }
}

float InputManager::getMouseDeltaX() {
    return dx;
}

float InputManager::getMouseDeltaY() {
    return dy;
}