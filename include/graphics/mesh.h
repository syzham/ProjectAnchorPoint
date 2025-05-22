#ifndef PROJECTANCHORPOINT_MESH_H
#define PROJECTANCHORPOINT_MESH_H

#include <vector>
#include <d3d11.h>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <filesystem>

struct Coords {
    float coord[3];
};

struct Vertex {
    Coords pos;
    Coords normal;
};

struct Material {
    float diffuseColor[4] = {1, 1, 1};
};


class Mesh {
public:
    std::vector<Vertex> vertices;
    Material material;
    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* materialCBuffer = nullptr;
    D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    void LoadFromOBJ(const std::string& filename, D3D11_PRIMITIVE_TOPOLOGY top);
    void CreateVertexBuffer(ID3D11Device* device);
    void Draw(ID3D11DeviceContext* context);
    void Release();

private:
    static std::unordered_map<std::string, Material> materials;
    static void LoadMTL(const std::string& mtlPath);
};

#endif //PROJECTANCHORPOINT_MESH_H
