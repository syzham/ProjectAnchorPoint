#ifndef ANCHORPOINT_ECS_STORAGE_H
#define ANCHORPOINT_ECS_STORAGE_H

#include <cstdint>
#include <utility>
#include <vector>

#include "anchorpoint/ecs/entity.h"

namespace ap {

// Type-erased interface so the World can manage pools generically.
class IComponentPool {
public:
    virtual ~IComponentPool() = default;
    virtual bool Has(std::uint32_t entityIndex) const = 0;
    virtual void Remove(std::uint32_t entityIndex) = 0;
    virtual void Clear() = 0;
};

// Sparse-set storage: components of one type live contiguously in a dense
// array for cache-friendly iteration, with a sparse array mapping entity
// index -> dense slot.
template<typename T>
class ComponentPool final : public IComponentPool {
public:
    template<typename... Args>
    T& Emplace(Entity entity, Args&&... args) {
        if (entity.index >= sparse.size())
            sparse.resize(entity.index + 1, 0);

        if (sparse[entity.index] != 0)
            return denseData[sparse[entity.index] - 1] = T{std::forward<Args>(args)...};

        denseEntities.push_back(entity);
        denseData.emplace_back(std::forward<Args>(args)...);
        sparse[entity.index] = static_cast<std::uint32_t>(denseData.size());
        return denseData.back();
    }

    bool Has(std::uint32_t entityIndex) const override {
        return entityIndex < sparse.size() && sparse[entityIndex] != 0;
    }

    T* TryGet(std::uint32_t entityIndex) {
        if (!Has(entityIndex)) return nullptr;
        return &denseData[sparse[entityIndex] - 1];
    }

    void Remove(std::uint32_t entityIndex) override {
        if (!Has(entityIndex)) return;

        const std::uint32_t denseIndex = sparse[entityIndex] - 1;
        const std::uint32_t lastIndex = static_cast<std::uint32_t>(denseData.size()) - 1;

        if (denseIndex != lastIndex) {
            denseData[denseIndex] = std::move(denseData[lastIndex]);
            denseEntities[denseIndex] = denseEntities[lastIndex];
            sparse[denseEntities[denseIndex].index] = denseIndex + 1;
        }

        denseData.pop_back();
        denseEntities.pop_back();
        sparse[entityIndex] = 0;
    }

    void Clear() override {
        sparse.clear();
        denseEntities.clear();
        denseData.clear();
    }

    std::size_t Size() const { return denseData.size(); }
    Entity EntityAt(std::size_t denseIndex) const { return denseEntities[denseIndex]; }
    T& DataAt(std::size_t denseIndex) { return denseData[denseIndex]; }

private:
    std::vector<std::uint32_t> sparse; // entity index -> dense index + 1 (0 = absent)
    std::vector<Entity> denseEntities;
    std::vector<T> denseData;
};

} // namespace ap

#endif //ANCHORPOINT_ECS_STORAGE_H
