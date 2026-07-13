#ifndef ANCHORPOINT_ECS_ENTITY_H
#define ANCHORPOINT_ECS_ENTITY_H

#include <cstdint>

namespace ap {

// A lightweight handle into the World. Entities own no data and have no
// behaviour: they are just an index plus a generation counter used to detect
// stale handles after an entity slot is reused.
struct Entity {
    static constexpr std::uint32_t kInvalidIndex = 0xFFFFFFFFu;

    std::uint32_t index = kInvalidIndex;
    std::uint32_t generation = 0;

    bool IsValid() const { return index != kInvalidIndex; }

    bool operator==(const Entity&) const = default;
};

} // namespace ap

#endif //ANCHORPOINT_ECS_ENTITY_H
