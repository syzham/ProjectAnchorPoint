#include <iostream>
#include "game/components/meshrenderer.h"


std::string MeshRenderer::getName() {
    return "MeshRenderer";
}

void MeshRenderer::Update() {
    mesh.Draw(GetWorldMatrix());
}

void MeshRenderer::Destroy() {
    mesh.Release();
}

void MeshRenderer::Init(nlohmann::basic_json<> data) {
    transform = owner->getComponent<Transform>();
    mesh.LoadFromOBJ(data["model"], ParseTopology(data["topology"]));
    mesh.CreateVertexBuffer();
}

DirectX::XMMATRIX MeshRenderer::GetWorldMatrix() const {

    DirectX::XMMATRIX trans = DirectX::XMMatrixTranslation(transform->position[0], transform->position[1], transform->position[2]);
    DirectX::XMMATRIX rotX = DirectX::XMMatrixRotationX(transform->rotation[0]);
    DirectX::XMMATRIX rotY = DirectX::XMMatrixRotationY(transform->rotation[1]);
    DirectX::XMMATRIX rotZ = DirectX::XMMatrixRotationZ(transform->rotation[2]);
    DirectX::XMMATRIX rot = rotZ * rotY * rotX;
    DirectX::XMMATRIX scl = DirectX::XMMatrixScaling(transform->scale[0], transform->scale[1], transform->scale[2]);
    return scl * rot * trans;
}

D3D11_PRIMITIVE_TOPOLOGY MeshRenderer::ParseTopology(const std::string &str) {
    if (str == "TRIANGLELIST") return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    if (str == "LINELIST") return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    if (str == "POINTLIST") return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
    return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}
