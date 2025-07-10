#include "graphics/mesh.h"

std::unordered_map<std::string, Material> Mesh::materials;


void Mesh::LoadFromOBJ(const std::string &filename, D3D11_PRIMITIVE_TOPOLOGY top) {
    topology = top;
    std::ifstream file(filename);
    std::vector<Coords> tempPosition;
    std::vector<UV> tempUv;
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
        } else if (type == "vt") {
            iss >> x >> y;
            tempUv.push_back({x, y});
        } else if (type == "vn") {
            iss >> x >> y >> z;
            tempNormal.push_back({x, y, z});
        } else if (type == "f") {
            std::vector<unsigned> fIndices;
            std::vector<unsigned> vtIndices;
            std::vector<unsigned> vnIndices;
            while (iss >> triplet) {
                std::istringstream vec(triplet);
                if (std::getline(vec, token, '/') && !token.empty())
                    fIndices.push_back(std::stoi(token));
                if (std::getline(vec, token, '/') && !token.empty())
                    vtIndices.push_back(std::stoi(token));
                if (std::getline(vec, token, '/') && !token.empty())
                    vnIndices.push_back(std::stoi(token));
            }
            for (size_t i = 1; i + 1 < fIndices.size(); ++i) {
                vertices.push_back({tempPosition[fIndices[0] - 1],
                                    (fIndices.size() == vnIndices.size()) ? tempNormal[vnIndices[0] - 1] : Coords({0, 0, 0}),
                                    (fIndices.size() == vtIndices.size()) ? tempUv[vtIndices[0] - 1] : UV({0, 0})});

                vertices.push_back({tempPosition[fIndices[i] - 1],
                                    (fIndices.size() == vnIndices.size()) ? tempNormal[vnIndices[i] - 1] : Coords({0, 0, 0}),
                                    (fIndices.size() == vtIndices.size()) ? tempUv[vtIndices[i] - 1] : UV({0, 0})});

                vertices.push_back({tempPosition[fIndices[i+1] - 1],
                                    (fIndices.size() == vnIndices.size()) ? tempNormal[vnIndices[i+1] - 1] : Coords({0, 0, 0}),
                                    (fIndices.size() == vtIndices.size()) ? tempUv[vtIndices[i+1] - 1] : UV({0, 0})});
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
    std::string line, currentName, textPath;

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
        } else if (token == "map_Kd") {
            iss >> textPath;
            std::filesystem::path base = std::filesystem::path(mtlPath).parent_path();
            Mesh::materials[currentName].texture = (base / textPath).string();
        }
    }
}

void Mesh::CreateVertexBuffer() {
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = sizeof(Vertex) * vertices.size();
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices.data();

    device->CreateBuffer(&desc, &initData, &vertexBuffer);


    D3D11_BUFFER_DESC matDesc = {};
    matDesc.ByteWidth = sizeof(MaterialBuffer);
    matDesc.Usage = D3D11_USAGE_DYNAMIC;
    matDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    device->CreateBuffer(&matDesc, nullptr, &materialCBuffer);

    DirectX::CreateWICTextureFromFile(device, std::wstring(material.texture.begin(), material.texture.end()).c_str(), nullptr, &textureSRV);

    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = sampDesc.AddressV = sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    device->CreateSamplerState(&sampDesc, &samplerState);
}

void Mesh::Draw(DirectX::XMMATRIX worldMatrix) {
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    dc->Map(objectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

    auto* objBuffer = reinterpret_cast<ObjectBuffer*>(mapped.pData);
    DirectX::XMStoreFloat4x4(&objBuffer->worldViewProj, DirectX::XMMatrixTranspose(worldMatrix * Camera::getMainCamera().GetViewMatrix() * Camera::getMainCamera().GetProjectionMatrix()));
    DirectX::XMStoreFloat4x4(&objBuffer->world, DirectX::XMMatrixTranspose(worldMatrix));

    dc->Unmap(objectBuffer, 0);
    dc->VSSetConstantBuffers(1, 1, &objectBuffer);

    D3D11_MAPPED_SUBRESOURCE matMapped = {};
    dc->Map(materialCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &matMapped);

    auto* materialBuffer = reinterpret_cast<MaterialBuffer*>(matMapped.pData);
    materialBuffer->diffuseColor[0] = material.diffuseColor[0];
    materialBuffer->diffuseColor[1] = material.diffuseColor[1];
    materialBuffer->diffuseColor[2] = material.diffuseColor[2];

    dc->Unmap(materialCBuffer, 0);
    dc->PSSetConstantBuffers(0, 1, &materialCBuffer);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    dc->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    dc->IASetPrimitiveTopology(topology);
    dc->Draw(static_cast<UINT>(vertices.size()), 0);

    dc->PSSetShaderResources(1, 1, &textureSRV);
    dc->PSSetSamplers(0, 1, &samplerState);
}

void Mesh::Release() {
    vertexBuffer->Release();
    materialCBuffer->Release();
    textureSRV->Release();
    samplerState->Release();
}