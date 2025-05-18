#include "graphics/scene.h"

void Scene::Load(const std::string& sceneFile, ID3D11Device *device) {
    std::ifstream file(sceneFile);
    std::string line, objPath, topology;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        iss >> objPath >> topology;
        if (objPath.empty()) continue;

        Mesh mesh;
        mesh.LoadFromOBJ(objPath, ParseTopology(topology));
        mesh.CreateVertexBuffer(device);
        meshes.push_back(std::move(mesh));
    }
}

void Scene::Draw(ID3D11DeviceContext *context) {
    for (auto& mesh : meshes) {
        mesh.Draw(context);
    }
}

void Scene::Unload() {
    for (auto& mesh : meshes) {
        mesh.Release();
    }
}

D3D11_PRIMITIVE_TOPOLOGY Scene::ParseTopology(const std::string &str) {
    if (str == "TRIANGLELIST") return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    if (str == "LINELIST") return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    if (str == "POINTLIST") return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
    return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}