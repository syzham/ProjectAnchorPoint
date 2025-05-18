#include "graphics/mesh.h"

void Mesh::LoadFromOBJ(const std::string &filename, D3D11_PRIMITIVE_TOPOLOGY top) {
    topology = top;
    std::ifstream file(filename);
    std::vector<Vertex> tempVertex;
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            tempVertex.push_back({x, y, z, 0.0f, 0.0f, 0.0f, 1.0f});
        } else if (type == "f") {
            unsigned int i1, i2, i3;
            iss >> i1 >> i2 >> i3;
            vertices.push_back(tempVertex[i1 - 1]);
            vertices.push_back(tempVertex[i2 - 1]);
            vertices.push_back(tempVertex[i3 - 1]);
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
}

void Mesh::Draw(ID3D11DeviceContext *context) {
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    context->IASetPrimitiveTopology(topology);
    context->Draw(static_cast<UINT>(vertices.size()), 0);
}

void Mesh::Release() {
    vertexBuffer->Release();
}