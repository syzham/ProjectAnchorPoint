#pragma once

#include "common.h"

struct AABBCollider {
    Vector3 min    = {0.0f, 0.0f, 0.0f};
    Vector3 max    = {0.0f, 0.0f, 0.0f};
    Vector3 size   = {1.0f, 1.0f, 1.0f};
    Vector3 offset = {0.0f, 0.0f, 0.0f};
    bool isStatic  = false;
};
