#ifndef PROJECTANCHORPOINT_SCRIPTEXPORT_H
#define PROJECTANCHORPOINT_SCRIPTEXPORT_H

#include "game/components/transform.h"
#include "game/InputManager.h"

#define DLL_EXPORT extern "C" __declspec(dllexport)


DLL_EXPORT void SetPosition(void* transform, float x, float y, float z);
DLL_EXPORT void SetRotation(void* transform, float x, float y, float z);
DLL_EXPORT void SetScale(void* transform, float x, float y, float z);
DLL_EXPORT void AddPosition(void* transform, float x, float y, float z);
DLL_EXPORT void AddRotation(void* transform, float x, float y, float z, bool yClamped);
DLL_EXPORT void AddScale(void* transform, float x, float y, float z);


DLL_EXPORT void getMouseMovement(void* inputManager, float* dx, float* dy);

#endif //PROJECTANCHORPOINT_SCRIPTEXPORT_H
