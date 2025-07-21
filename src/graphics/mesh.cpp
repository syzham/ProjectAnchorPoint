#include "graphics/mesh.h"

std::unordered_map<std::string, Material> Mesh::materials;


void Mesh::LoadFromOBJ(const std::string &filename, D3D11_PRIMITIVE_TOPOLOGY top) {
    topology = top;
    std::ifstream file(filename);
    std::vector<Vector3> tempPosition;
    std::vector<UV> tempUv;
    std::vector<Vector3> tempNormal;
    std::string line, triplet, token;
    float x, y, z;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type, mtlName;
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
                                    (fIndices.size() == vnIndices.size()) ? tempNormal[vnIndices[0] - 1] : Vector3({0, 0, 0}),
                                    (fIndices.size() == vtIndices.size()) ? tempUv[vtIndices[0] - 1] : UV({0, 0})});

                vertices.push_back({tempPosition[fIndices[i] - 1],
                                    (fIndices.size() == vnIndices.size()) ? tempNormal[vnIndices[i] - 1] : Vector3({0, 0, 0}),
                                    (fIndices.size() == vtIndices.size()) ? tempUv[vtIndices[i] - 1] : UV({0, 0})});

                vertices.push_back({tempPosition[fIndices[i+1] - 1],
                                    (fIndices.size() == vnIndices.size()) ? tempNormal[vnIndices[i+1] - 1] : Vector3({0, 0, 0}),
                                    (fIndices.size() == vtIndices.size()) ? tempUv[vtIndices[i+1] - 1] : UV({0, 0})});
            }
        } else if (type == "usemtl") {
            iss >> mtlName;
            if (!Mesh::materials.contains(mtlName))
                LoadMTRL(mtlName);

            material = &Mesh::materials[mtlName];

        }
    }
}

void Mesh::LoadMTRL(const std::string &mtlName) {
    std::ifstream in("assets/materials/" + mtlName + ".mtrl");
    if (!in)
        return;

    dat mats;
    in >> mats;

    Material tempMat;
    tempMat.diffuseColor = {mats["diffuseColor"][0], mats["diffuseColor"][1], mats["diffuseColor"][2]};
    std::string vs = mats["shader"]["vertex"].get<std::string>();
    std::string ps = mats["shader"]["pixel"].get<std::string>();
    tempMat.vs = std::wstring(vs.begin(), vs.end());
    tempMat.ps = std::wstring(ps.begin(), ps.end());
    tempMat.texture = mats["texture"];

    Mesh::materials[mtlName] = tempMat;
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

    DirectX::CreateWICTextureFromFile(device, std::wstring(material->texture.begin(), material->texture.end()).c_str(), nullptr, &textureSRV);

    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = sampDesc.AddressV = sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    device->CreateSamplerState(&sampDesc, &samplerState);

    shader.Load(device, material->vs, material->ps);
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
    materialBuffer->diffuseColor[0] = material->diffuseColor.x;
    materialBuffer->diffuseColor[1] = material->diffuseColor.y;
    materialBuffer->diffuseColor[2] = material->diffuseColor.z;

    dc->Unmap(materialCBuffer, 0);
    dc->PSSetConstantBuffers(0, 1, &materialCBuffer);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    dc->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    dc->IASetPrimitiveTopology(topology);

    dc->PSSetShaderResources(1, 1, &textureSRV);
    dc->PSSetSamplers(0, 1, &samplerState);

    shader.Bind(dc);

    dc->Draw(static_cast<UINT>(vertices.size()), 0);
}

void Mesh::Release() {
    vertexBuffer->Release();
    materialCBuffer->Release();
    textureSRV->Release();
    samplerState->Release();
    shader.Release();
}