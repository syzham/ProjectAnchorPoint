#ifndef ANCHORPOINT_ECS_WORLD_H
#define ANCHORPOINT_ECS_WORLD_H

#include <cassert>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "anchorpoint/ecs/entity.h"
#include "anchorpoint/ecs/storage.h"

namespace ap {

// The component registry at the heart of the ECS. Entities are created and
// destroyed here, components are attached as plain data, and systems iterate
// them with Each<>(). The World is header-only and has no platform
// dependencies.
class World {
public:
    Entity CreateEntity() {
        if (!freeList.empty()) {
            const std::uint32_t index = freeList.back();
            freeList.pop_back();
            alive[index] = true;
            return {index, generations[index]};
        }
        const auto index = static_cast<std::uint32_t>(generations.size());
        generations.push_back(0);
        alive.push_back(true);
        return {index, 0};
    }

    void DestroyEntity(Entity entity) {
        if (!IsAlive(entity)) return;
        for (auto& [type, pool] : pools)
            pool->Remove(entity.index);
        alive[entity.index] = false;
        ++generations[entity.index];
        freeList.push_back(entity.index);
    }

    bool IsAlive(Entity entity) const {
        return entity.IsValid()
            && entity.index < generations.size()
            && alive[entity.index]
            && generations[entity.index] == entity.generation;
    }

    // Destroys every entity and component (used when unloading a scene).
    void Clear() {
        pools.clear();
        generations.clear();
        alive.clear();
        freeList.clear();
    }

    template<typename T, typename... Args>
    T& Add(Entity entity, Args&&... args) {
        assert(IsAlive(entity) && "Adding a component to a dead entity");
        return Pool<T>().Emplace(entity, std::forward<Args>(args)...);
    }

    template<typename T>
    void Remove(Entity entity) {
        if (auto* pool = TryPool<T>())
            pool->Remove(entity.index);
    }

    template<typename T>
    bool Has(Entity entity) const {
        const auto* pool = TryPool<T>();
        return pool != nullptr && IsAlive(entity) && pool->Has(entity.index);
    }

    template<typename T>
    T* TryGet(Entity entity) {
        auto* pool = TryPool<T>();
        if (pool == nullptr || !IsAlive(entity)) return nullptr;
        return pool->TryGet(entity.index);
    }

    template<typename T>
    T& Get(Entity entity) {
        T* component = TryGet<T>(entity);
        assert(component && "Entity does not have the requested component");
        return *component;
    }

    // Invokes fn(Entity, First&, Rest&...) for every entity that has all the
    // listed components. Do not create or destroy entities of the iterated
    // types from inside the callback.
    template<typename First, typename... Rest, typename Fn>
    void Each(Fn&& fn) {
        auto& pool = Pool<First>();
        for (std::size_t i = 0; i < pool.Size(); ++i) {
            const Entity entity = pool.EntityAt(i);
            if constexpr (sizeof...(Rest) > 0) {
                if (!(Has<Rest>(entity) && ...)) continue;
                fn(entity, pool.DataAt(i), Get<Rest>(entity)...);
            } else {
                fn(entity, pool.DataAt(i));
            }
        }
    }

private:
    template<typename T>
    ComponentPool<T>& Pool() {
        auto& slot = pools[std::type_index(typeid(T))];
        if (!slot) slot = std::make_unique<ComponentPool<T>>();
        return *static_cast<ComponentPool<T>*>(slot.get());
    }

    template<typename T>
    const ComponentPool<T>* TryPool() const {
        const auto it = pools.find(std::type_index(typeid(T)));
        if (it == pools.end()) return nullptr;
        return static_cast<const ComponentPool<T>*>(it->second.get());
    }

    template<typename T>
    ComponentPool<T>* TryPool() {
        const auto it = pools.find(std::type_index(typeid(T)));
        if (it == pools.end()) return nullptr;
        return static_cast<ComponentPool<T>*>(it->second.get());
    }

    std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> pools;
    std::vector<std::uint32_t> generations;
    std::vector<bool> alive;
    std::vector<std::uint32_t> freeList;
};

} // namespace ap

#endif //ANCHORPOINT_ECS_WORLD_H
