#ifndef ANCHORPOINT_SYSTEMS_COLLISION_SYSTEM_H
#define ANCHORPOINT_SYSTEMS_COLLISION_SYSTEM_H

#include <vector>

#include "anchorpoint/components/collider.h"
#include "anchorpoint/core/api.h"
#include "anchorpoint/ecs/entity.h"
#include "anchorpoint/ecs/system.h"
#include "anchorpoint/math/math.h"

namespace ap {

// Resolves dynamic AABBCollider entities against static ones using a uniform
// grid over the XZ plane. Added automatically by Engine::Init().
class AP_API CollisionSystem : public System {
public:
    static constexpr int kGridSize = 100;
    static constexpr float kCellSize = 10.0f;

    void OnUpdate(Engine& engine) override;

private:
    struct StaticBox {
        Entity entity;
        Vector3 min, max, size;
    };

    struct GridCell {
        std::vector<StaticBox> staticGeometry;
    };

    int CellIndex(int gx, int gz) const { return gz * kGridSize + gx; }
    void AddStatic(const StaticBox& box, int gx, int gz);
    void QueryNearby(int minX, int maxX, int minZ, int maxZ,
                     std::vector<const StaticBox*>& out) const;

    std::vector<GridCell> grid = std::vector<GridCell>(kGridSize * kGridSize);
};

} // namespace ap

#endif //ANCHORPOINT_SYSTEMS_COLLISION_SYSTEM_H
