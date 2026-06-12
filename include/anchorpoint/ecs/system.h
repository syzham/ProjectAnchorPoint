#ifndef ANCHORPOINT_ECS_SYSTEM_H
#define ANCHORPOINT_ECS_SYSTEM_H

namespace ap {

class Engine;

// All behaviour lives in systems. Implement one or more of the callbacks and
// register the system with Engine::AddSystem<T>(). Systems run in the order
// they were added; the engine appends its built-in collision and render
// systems during Init(), so gameplay systems added before Init() run first
// each frame.
class System {
public:
    virtual ~System() = default;

    virtual void OnInit(Engine& engine) {}
    virtual void OnUpdate(Engine& engine) {}
    virtual void OnFixedUpdate(Engine& engine) {}
    virtual void OnShutdown(Engine& engine) {}
};

} // namespace ap

#endif //ANCHORPOINT_ECS_SYSTEM_H
