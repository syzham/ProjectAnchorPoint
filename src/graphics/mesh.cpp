#include "graphics/mesh.h"

std::unordered_map<std::string, Material> Mesh::materials;

void Mesh::LoadFromOBJ(const std::string &filename, D3D11_PRIMITIVE_TOPOLOGY top) {
    topology = top;
    std::ifstream file(filename);
    std::vector<Coords> tempPosition;
    std::vector<Coords> tempNormal;
    std::string line, triplet, token;
    float x, y, z;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type, mtlPath, mtlName;
        iss >> type;

        if (type == "v") {
            iss >> x >> y >> z;
            tempPosition.push_back({x, y, z});
        } else if (type == "vn") {
            iss >> x >> y >> z;
            tempNormal.push_back({x, y, z});
        } else if (type == "f") {
            unsigned int i, vt, vn;
            while (iss >> triplet) {
                std::istringstream vec(triplet);
                if (std::getline(vec, token, '/') && !token.empty())
                    i = std::stoi(token);
                if (std::getline(vec, token, '/') && !token.empty())
                    vt = std::stoi(token);
                if (std::getline(vec, token, '/') && !token.empty())
                    vn = std::stoi(token);
                vertices.push_back({{tempPosition[i - 1]}, {tempNormal[vn - 1]}});
            }
        } else if (type == "mtllib") {
            iss >> mtlPath;
            std::filesystem::path base = std::filesystem::path(filename).parent_path();
            Mesh::LoadMTL((base / mtlPath).string());
        } else if (type == "usemtl") {
            iss >> mtlName;
            material = Mesh::materials[mtlName];
        }
    }
}

void Mesh::LoadMTL(const std::string &mtlPath) {
    std::ifstream file(mtlPath);
    std::string line, currentName;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "newmtl") {
            iss >> currentName;
            if (Mesh::materials.contains(currentName))
                break;

            Mesh::materials[currentName] = Material { };
        } else if (token == "Kd") {
            float r, g, b;
            iss >> r >> g >> b;
            Mesh::materials[currentName].diffuseColor[0] = r;
            Mesh::materials[currentName].diffuseColor[1] = g;
            Mesh::materials[currentName].diffuseColor[2] = b;
        }
    }
}

void Mesh::CreateVertexBuffer(ID3D11Device *device) {
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = sizeof(Vertex) * vertices.size();
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices.data();

    device->CreateBuffer(&desc, &initData, &vertexBuffer);


    D3D11_BUFFER_DESC matDesc = {};
    matDesc.ByteWidth = sizeof(Material);
    matDesc.Usage = D3D11_USAGE_DYNAMIC;
    matDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    device->CreateBuffer(&matDesc, nullptr, &materialCBuffer);
}

void Mesh::Draw(ID3D11DeviceContext *context) {
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    context->Map(materialCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

    auto* buffer = reinterpret_cast<Material*>(mapped.pData);
    buffer->diffuseColor[0] = material.diffuseColor[0];
    buffer->diffuseColor[1] = material.diffuseColor[1];
    buffer->diffuseColor[2] = material.diffuseColor[2];

    context->Unmap(materialCBuffer, 0);
    context->PSSetConstantBuffers(0, 1, &materialCBuffer);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    context->IASetPrimitiveTopology(topology);
    context->Draw(static_cast<UINT>(vertices.size()), 0);
}

void Mesh::Release() {
    vertexBuffer->Release();
    materialCBuffer->Release();
}