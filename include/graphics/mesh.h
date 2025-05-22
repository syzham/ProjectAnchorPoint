#ifndef PROJECTANCHORPOINT_MESH_H
#define PROJECTANCHORPOINT_MESH_H

#include <vector>
#include <d3d11.h>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <filesystem>
#include <WICTextureLoader11.h>

struct Coords {
    float x, y, z;
};

struct UV {
    float u, v;
};

struct Vertex {
    Coords pos;
    Coords normal;
    UV uv;
};

struct Material {
    float diffuseColor[3] = {1, 1, 1};
    std::string texture;
};


class Mesh {
public:
    std::vector<Vertex> vertices;
    Material material;
    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* materialCBuffer = nullptr;
    ID3D11ShaderResourceView* textureSRV = nullptr;
    ID3D11SamplerState* samplerState = nullptr;
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
