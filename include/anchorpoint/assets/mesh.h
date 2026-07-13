#ifndef ANCHORPOINT_ASSETS_MESH_H
#define ANCHORPOINT_ASSETS_MESH_H

#include <string>
#include <vector>

#include "anchorpoint/core/api.h"
#include "anchorpoint/math/math.h"

namespace ap {

enum class PrimitiveTopology {
    TriangleList,
    LineList,
    PointList,
};

AP_API PrimitiveTopology ParseTopology(const std::string& str);

struct Vertex {
    Vector3 position;
    Vector3 normal;
    // Tangent of the UV mapping, used for normal mapping. Computed per
    // triangle by LoadOBJ; defaults to +X when the mesh has no UVs.
    Vector3 tangent = {1, 0, 0};
    float u = 0, v = 0;
};

struct MaterialData {
    Vector3 diffuseColor = {1, 1, 1};
    std::string vertexShader;
    std::string pixelShader;
    std::string texture;
    // Optional tangent-space normal map; empty means a flat surface.
    std::string normalMap;
};

// CPU-side mesh data, fully platform independent. Renderer backends turn
// this into GPU resources via Renderer::CreateMesh().
struct MeshData {
    std::vector<Vertex> vertices;
    MaterialData material;
    PrimitiveTopology topology = PrimitiveTopology::TriangleList;
};

// Loads a Wavefront OBJ (triangulating faces) plus the JSON material file
// referenced by its "usemtl" statement (assets/materials/<name>.mtrl).
AP_API bool LoadOBJ(const std::string& filename, MeshData& out);

} // namespace ap

#endif //ANCHORPOINT_ASSETS_MESH_H
