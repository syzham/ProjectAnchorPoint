#ifndef PROJECTANCHORPOINT_INPUTMANAGER_H
#define PROJECTANCHORPOINT_INPUTMANAGER_H

#include <Windows.h>
#include "graphics/camera.h"
#include "core/config.h"

class InputManager {
public:
    static InputManager& getInstance() {
        static InputManager instance;
        return instance;
    }

    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;
    InputManager(InputManager&&) = delete;
    InputManager& operator=(const InputManager&&) = delete;

    void MouseEvent(HWND hwnd);

    float getMouseDeltaX();
    float getMouseDeltaY();

private:
    InputManager() = default;
    float dx;
    float dy;
};

#endif //PROJECTANCHORPOINT_INPUTMANAGER_H
