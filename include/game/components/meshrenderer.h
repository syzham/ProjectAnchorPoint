#pragma once

#include "graphics/mesh.h"
#include <d3d11.h>
#include <string>

struct MeshRenderer {
    Mesh mesh;
    D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    bool initialized = false;

    static D3D11_PRIMITIVE_TOPOLOGY ParseTopology(const std::string& str);
};
