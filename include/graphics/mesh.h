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
#include "graphics/camera.h"
#include "graphics/graphics.h"
#include "common.h"
#include "nlohmann/json.hpp"
using dat = nlohmann::json;

struct UV {
    float u, v;
};

struct Vertex {
    Vector3 pos;
    Vector3 normal;
    UV uv;
};

struct MaterialBuffer {
    float diffuseColor[4];
};

struct Material {
    Vector3 diffuseColor;
    std::wstring vs;
    std::wstring ps;
    std::string texture;
};


class Mesh {
public:
    std::vector<Vertex> vertices;
    Material* material;
    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* materialCBuffer = nullptr;
    ID3D11ShaderResourceView* textureSRV = nullptr;
    ID3D11SamplerState* samplerState = nullptr;
    D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    Shader shader;

    void LoadFromOBJ(const std::string& filename, D3D11_PRIMITIVE_TOPOLOGY top);
    void CreateVertexBuffer();
    void Draw(DirectX::XMMATRIX worldMatrix);
    void Release();

private:
    static std::unordered_map<std::string, Material> materials;
    static void LoadMTRL(const std::string& mtlName);
};

#endif //PROJECTANCHORPOINT_MESH_H
