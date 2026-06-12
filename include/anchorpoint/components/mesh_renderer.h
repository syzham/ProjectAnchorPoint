#ifndef ANCHORPOINT_COMPONENTS_MESH_RENDERER_H
#define ANCHORPOINT_COMPONENTS_MESH_RENDERER_H

#include <string>

#include "anchorpoint/assets/mesh.h"
#include "anchorpoint/platform/renderer.h"

namespace ap {

struct MeshRenderer {
    std::string model; // OBJ path, relative to the working directory
    PrimitiveTopology topology = PrimitiveTopology::TriangleList;

    // Filled in lazily by the RenderSystem.
    MeshHandle gpuMesh = kInvalidMesh;
    bool loadFailed = false;
};

} // namespace ap

#endif //ANCHORPOINT_COMPONENTS_MESH_RENDERER_H
