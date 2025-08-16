#ifndef PROJECTANCHORPOINT_COLLISIONMANAGER_H
#define PROJECTANCHORPOINT_COLLISIONMANAGER_H

#include <vector>
#include "common.h"

class AACollider;

constexpr int GRID_SIZE = 100;
constexpr float CELL_SIZE = 10.0f;

struct GridCell {
    std::vector<const AACollider*> staticGeometry;
};

class StaticWorld {
public:
    std::vector<GridCell> grid;

    static StaticWorld& getInstance() {
        static StaticWorld instance;
        instance.grid.resize(GRID_SIZE * GRID_SIZE);
        return instance;
    }

    int GetIndex(int gx, int gz) const;

    void AddStatic(const AACollider* box, int gx, int gz);
    std::vector<const AACollider*> QueryNearby(int minX, int maxX, int minZ, int maxZ) const;
};

#endif //PROJECTANCHORPOINT_COLLISIONMANAGER_H
