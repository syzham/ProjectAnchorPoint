#include "game/ScriptExport.h"

void QuitGame() {
    PostQuitMessage(0);
}

void SetPosition(void* transform, float x, float y, float z) {
    ((Transform*)transform)->SetPosition({x, y, z});
}

void SetRotation(void* transform, float x, float y, float z) {
    ((Transform*)transform)->SetRotation({x, y, z});
}

void SetScale(void* transform, float x, float y, float z) {
    ((Transform*)transform)->SetScale({x, y, z});
}

void AddPosition(void* transform, float x, float y, float z) {
    ((Transform*)transform)->AddPosition({x, y, z});
}

void AddRotation(void* transform, float x, float y, float z, bool yClamped) {
    ((Transform*)transform)->AddRotation({x, y, z}, yClamped);
}

void AddScale(void* transform, float x, float y, float z) {
    ((Transform*)transform)->AddScale({x, y, z});
}

void MoveForwards(void* transform, float speed) {
    ((Transform*)transform)->MoveForwards(speed);
}

void MoveRight(void* transform, float speed) {
    ((Transform*)transform)->MoveRight(speed);
}

void getMouseMovement(void* inputManager, float* dx, float* dy) {
    *dx = ((InputManager*)inputManager)->getMouseDeltaX();
    *dy = ((InputManager*)inputManager)->getMouseDeltaY();
}

void isKeyDown(void *inputManager, int keycode, bool* isDown) {
    *isDown = ((InputManager*)inputManager)->isKeyDown(keycode);
}
