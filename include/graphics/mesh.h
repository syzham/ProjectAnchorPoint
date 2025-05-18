#ifndef PROJECTANCHORPOINT_MESH_H
#define PROJECTANCHORPOINT_MESH_H

#include <vector>
#include <d3d11.h>
#include <string>
#include <fstream>
#include <sstream>

struct Vertex {
    float x, y, z;
    float r, g, b, a;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    ID3D11Buffer* vertexBuffer = nullptr;
    D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    void LoadFromOBJ(const std::string& filename, D3D11_PRIMITIVE_TOPOLOGY top);
    void CreateVertexBuffer(ID3D11Device* device);
    void Draw(ID3D11DeviceContext* context);
    void Release();
};

#endif //PROJECTANCHORPOINT_MESH_H
