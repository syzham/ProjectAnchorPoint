#include "game/components/meshrenderer.h"

D3D11_PRIMITIVE_TOPOLOGY MeshRenderer::ParseTopology(const std::string& str) {
    if (str == "TRIANGLELIST") return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    if (str == "LINELIST")     return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    if (str == "POINTLIST")    return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
    return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}
