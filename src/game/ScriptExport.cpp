#include <iostream>
#include "game/ScriptExport.h"

void SetPosition(void* transform, float x, float y, float z) {
    ((Transform*)transform)->SetPosition(x, y, z);
}

void SetRotation(void* transform, float x, float y, float z) {
    ((Transform*)transform)->SetRotation(x, y, z);
}

void SetScale(void* transform, float x, float y, float z) {
    ((Transform*)transform)->SetScale(x, y, z);
}

void AddPosition(void* transform, float x, float y, float z) {
    ((Transform*)transform)->AddPosition(x, y, z);
}

void AddRotation(void* transform, float x, float y, float z) {
    ((Transform*)transform)->AddRotation(x, y, z);
}

void AddScale(void* transform, float x, float y, float z) {
    ((Transform*)transform)->AddScale(x, y, z);
}