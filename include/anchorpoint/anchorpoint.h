// Umbrella header for the AnchorPoint game engine library.
#ifndef ANCHORPOINT_ANCHORPOINT_H
#define ANCHORPOINT_ANCHORPOINT_H

#include "anchorpoint/core/api.h"
#include "anchorpoint/core/config.h"
#include "anchorpoint/core/engine.h"
#include "anchorpoint/core/log.h"
#include "anchorpoint/core/time.h"

#include "anchorpoint/math/math.h"

#include "anchorpoint/ecs/entity.h"
#include "anchorpoint/ecs/storage.h"
#include "anchorpoint/ecs/system.h"
#include "anchorpoint/ecs/world.h"

#include "anchorpoint/platform/input.h"
#include "anchorpoint/platform/key.h"
#include "anchorpoint/platform/renderer.h"
#include "anchorpoint/platform/window.h"

#include "anchorpoint/assets/mesh.h"

#include "anchorpoint/components/camera.h"
#include "anchorpoint/components/collider.h"
#include "anchorpoint/components/light.h"
#include "anchorpoint/components/mesh_renderer.h"
#include "anchorpoint/components/name.h"
#include "anchorpoint/components/transform.h"

#include "anchorpoint/scene/scene_loader.h"

#include "anchorpoint/systems/collision_system.h"
#include "anchorpoint/systems/render_system.h"

#endif //ANCHORPOINT_ANCHORPOINT_H
