#ifndef PROJECTANCHORPOINT_SCRIPTBEHAVIOUR_H
#define PROJECTANCHORPOINT_SCRIPTBEHAVIOUR_H

#include <nethost.h>
#include <coreclr_delegates.h>
#include <hostfxr.h>
#include <iostream>
#include <windows.h>
#include "game/SceneManager.h"
#include "game/InputManager.h"

#pragma comment(lib, "nethost.lib")
extern hostfxr_handle cxt;
extern hostfxr_close_fn close_fn;
typedef void (CORECLR_DELEGATE_CALLTYPE* update_entry_point)();
extern void (*update_fn)();
typedef void (CORECLR_DELEGATE_CALLTYPE* create_entry_point)(const char*);
extern void (*create_fn)(const char*);
typedef void (CORECLR_DELEGATE_CALLTYPE* setPointer_entry_point)(const char*, void*);
extern void (*setPointer_fn)(const char*, void*);
typedef void (CORECLR_DELEGATE_CALLTYPE* setFloat_entry_point)(const char*, float);
extern void (*setFloat_fn)(const char*, float);
typedef void (CORECLR_DELEGATE_CALLTYPE* setVector3_entry_point)(const char*, float[3]);
extern void (*setVector3_fn)(const char*, float[3]);

int InitHost();
void UpdateScript();
void CloseHost();

#endif //PROJECTANCHORPOINT_SCRIPTBEHAVIOUR_H
