#pragma once

// ECS core
#include "core/Registry.h"
#include "core/System.h"

// Engine
#include "core/gameEngine.h"

// Built-in components
#include "game/components/transform.h"
#include "game/components/meshrenderer.h"
#include "game/components/lightcomponent.h"
#include "game/components/cameracomponent.h"
#include "game/components/colliders/aacollider.h"
#include "game/components/scriptcomponent.h"

// Built-in systems
#include "systems/RenderSystem.h"
#include "systems/CameraSystem.h"
#include "systems/LightSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/ScriptSystem.h"
