# Project Anchor Point

AnchorPoint is a pure-ECS game engine packaged as a C++ library. You build it
once, drop the headers, import library and DLL/shared library into your
project, and implement your game entirely in C++ by defining components (plain
data) and systems (behaviour).

## Highlights

- **Library, not a framework executable** — `cmake --install` produces a
  classic SDK layout (`include/`, `lib/`, `bin/`) plus a CMake package, so
  consumers just `find_package(AnchorPoint)` and link `AnchorPoint::anchorpoint`.
- **Pure ECS** — entities are IDs, components are plain structs stored in
  cache-friendly sparse sets, and all behaviour lives in systems scheduled by
  the engine (`OnInit` / `OnUpdate` / `OnFixedUpdate` / `OnShutdown`).
- **C++ scripting only** — the .NET/C# hosting layer is gone. Games implement
  systems and components in C++ and register scene loaders for their own
  component types.
- **Cross-platform core** — the ECS, math, scene loading, input, collision and
  the main loop are platform-independent. Windowing and rendering sit behind
  `ap::Window` / `ap::Renderer` interfaces with these backends:

  | Platform | Window | Renderer |
  |---|---|---|
  | Windows | Win32 | Direct3D 11 |
  | macOS | Cocoa (AppKit) | Metal |
  | any (headless) | null | null |

  The headless null backend (no window, no GPU) is used for servers, tests and
  CI, and is the fallback on platforms without a native backend yet. Linux
  windowed rendering (OpenGL/Vulkan) can be added by implementing the same two
  interfaces.

## Building the library

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
cmake --install build --config Release --prefix <sdk-dir>
```

Options:

| Option | Default | Description |
|---|---|---|
| `ANCHORPOINT_BUILD_SHARED` | `ON` | Build a DLL/shared library instead of a static library |
| `ANCHORPOINT_BUILD_EXAMPLES` | `ON` | Build the `examples/sandbox` demo game |

The install prefix ends up containing:

```
include/anchorpoint/...   public headers (plus bundled nlohmann/json)
lib/anchorpoint.lib       import/static library (libanchorpoint.so/.a elsewhere)
bin/anchorpoint.dll       the DLL (Windows shared build)
lib/cmake/AnchorPoint/    CMake package for find_package()
```

## Using the library

```cmake
find_package(AnchorPoint REQUIRED)
target_link_libraries(my_game PRIVATE AnchorPoint::anchorpoint)
```

```cpp
#include <anchorpoint/anchorpoint.h>

// Components are plain data.
struct Spin { ap::Vector3 delta; };

// Behaviour lives in systems.
class SpinSystem : public ap::System {
    void OnUpdate(ap::Engine& engine) override {
        engine.GetWorld().Each<ap::Transform, Spin>(
            [](ap::Entity, ap::Transform& transform, Spin& spin) {
                ap::AddRotation(transform, spin.delta);
            });
    }
};

int main() {
    ap::EngineConfig config;
    config.startScene = "first"; // loads scenes/first.scene

    ap::Engine engine(config);

    // Make the component loadable from .scene files.
    engine.GetScenes().RegisterComponent("Spin",
        [](ap::World& world, ap::Entity entity, const ap::SceneLoader::json& data) {
            world.Add<Spin>(entity, Spin{{data["delta"][0], data["delta"][1], data["delta"][2]}});
        });

    engine.AddSystem<SpinSystem>();

    if (engine.Init() != 0) return -1;
    engine.Run();
    return 0;
}
```

See `examples/sandbox/` for a complete game with mouse-look, WASD movement
(the old C# `PlayerController` ported to a C++ system) and collision.

### Example layout & game states

The sandbox is split so `main.cpp` stays a readable wiring-up of the game:

```
examples/sandbox/
  main.cpp            wires components + systems together
  components/         plain-data components (+ their scene loaders)
  systems/            behaviour, one System per file
  game/               the game-state machine
```

Systems can declare which game state they run in. This stays pure-ECS: the
active state is a singleton component (`GameStateContext`) in the World, and a
small `StateSystem` base gates `OnUpdate`/`OnFixedUpdate` with a query — no
scheduler or engine changes. A system "registers" its states through the base
constructor:

```cpp
class PlayerControllerSystem : public StateSystem {
public:
    PlayerControllerSystem() : StateSystem({GameState::Playing}) {}   // only while Playing
protected:
    void OnUpdateInState(ap::Engine& engine) override { /* ... */ }
};
```

An empty state list (or deriving from `ap::System` directly) means "run in
every state" — used by the pause and debug-toggle systems. In the sandbox,
**P** pauses/resumes (gameplay systems stop, the rest keep running) and **B**
toggles the collider overlay.

### Built-in components and systems

| Component | Scene key | Purpose |
|---|---|---|
| `ap::Name` | `Name` | Display name |
| `ap::Transform` | `Transform` | Position / rotation / scale |
| `ap::MeshRenderer` | `MeshRenderer` | OBJ model + topology |
| `ap::Camera` | `Camera` | Perspective camera (first active one is used) |
| `ap::Light` | `Light` | Directional / point / spot light data |
| `ap::AABBCollider` | `AABBCollider` | Axis-aligned box collider |

The engine appends its `CollisionSystem` and `RenderSystem` during
`Engine::Init()`, so gameplay systems added before `Init()` run first each
frame and the frame is rendered last.

### Shaders

The Windows/D3D11 backend compiles the HLSL in `shaders/` (`VS.hlsl` /
`PS.hlsl`, referenced from `.mtrl` material files, plus `DebugVS.hlsl` /
`DebugPS.hlsl` for the collider overlay) at runtime. The macOS/Metal backend
compiles `shaders/Shader.metal` (an MSL port of the same lighting plus the
debug line shaders) at runtime. Ship the `shaders/` directory alongside your
executable for whichever platform you target.

### Lighting, normal maps and shadows

Lighting is Lambertian over the scene's `Light` components with a small
ambient term, plus:

- **Normal maps** — add a `"normalMap"` entry to a `.mtrl` material to give
  meshes per-pixel surface detail. Tangents are generated automatically when
  the OBJ is loaded; materials without a normal map render flat as before.

  ```json
  {
    "shader": { "vertex": "shaders/VS.hlsl", "pixel": "shaders/PS.hlsl" },
    "diffuseColor": [1.0, 1.0, 1.0],
    "texture": "assets/textures/bricks.jpg",
    "normalMap": "assets/textures/bricks_normal.png"
  }
  ```

- **Shadows** — the first directional light casts real shadows: each frame a
  depth-only pass renders the scene from the light into a shadow map
  (`EngineConfig::shadowMapSize`, default 2048), and the main pass samples it
  with a 3x3 PCF filter, so objects cast soft-edged shadows onto each other
  instead of every away-facing side simply going dark. Scenes without a
  directional light skip the pass entirely.

### Debug collider overlay

A debug mode draws a wireframe box around every `AABBCollider` — green for
static colliders, red for dynamic ones — so you can see collision volumes
against the rendered meshes. Enable it from `EngineConfig`:

```cpp
config.debugDrawColliders = true;
```

or toggle it at runtime:

```cpp
engine.SetDebugDrawColliders(!engine.IsDebugDrawColliders());
```

The sandbox binds this to the **B** key (and accepts `--debug-colliders` to
start with it on). The overlay is rendered by the built-in `RenderSystem`, so
it works on both the D3D11 and Metal backends; the headless backend ignores
it. To visualise your own volumes, push lines into `FrameData::debugLines`
(see `ap::AppendAABBWireframe`).

### Headless mode

Set `EngineConfig::headless = true` (the sandbox accepts
`--headless --frames N`) to run without a window or GPU — handy for CI,
tests and dedicated servers, and the default on platforms without a native
rendering backend yet.

## Repository layout

```
include/anchorpoint/   public headers (ECS, math, components, systems, platform interfaces)
src/                   library implementation
src/platform/win32     Win32 window backend          (Windows only)
src/platform/d3d11     Direct3D 11 renderer backend  (Windows only)
src/platform/cocoa     Cocoa window backend          (macOS only)
src/platform/metal     Metal renderer backend        (macOS only)
src/platform/null      headless window/renderer backend
examples/sandbox/      demo game using the library
assets/, scenes/,      demo content used by the sandbox
shaders/
```

## To Do

- [ ] OpenGL or Vulkan renderer backend for Linux windowed rendering
- [ ] Physics component
- [ ] Resolution changes at runtime
- [ ] View-frustum / occlusion / back-face culling
- [ ] Animation system
- [ ] Audio system
