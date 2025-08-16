#include "game/CollisionManager.h"

int StaticWorld::GetIndex(int gx, int gz) const {
    return gz * GRID_SIZE + gx;
}

void StaticWorld::AddStatic(const AACollider* box, int gx, int gz) {
    if (gx < 0 || gx >= GRID_SIZE || gz < 0 || gz >= GRID_SIZE) return;
    grid[GetIndex(gx, gz)].staticGeometry.push_back(box);
}

std::vector<const AACollider*> StaticWorld::QueryNearby(int minX, int maxX, int minZ, int maxZ) const {
    std::vector<const AACollider*> result;

    for (int gz = minZ; gz <= maxZ; ++gz) {
        for (int gx = minX; gx <= maxX; ++gx) {
            if (gx < 0 || gx >= GRID_SIZE || gz < 0 || gz >= GRID_SIZE) continue;
            for (const auto& box : grid[GetIndex(gx, gz)].staticGeometry) {
                result.push_back(box);
            }
        }
    }
    return result;
}
