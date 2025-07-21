#include "game/components/meshrenderer.h"

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

    DirectX::XMMATRIX trans = DirectX::XMMatrixTranslation(transform->position.x, transform->position.y, transform->position.z);
    DirectX::XMMATRIX rotX = DirectX::XMMatrixRotationX(transform->rotation.x);
    DirectX::XMMATRIX rotY = DirectX::XMMatrixRotationY(transform->rotation.y);
    DirectX::XMMATRIX rotZ = DirectX::XMMatrixRotationZ(transform->rotation.z);
    DirectX::XMMATRIX rot = rotZ * rotY * rotX;
    DirectX::XMMATRIX scl = DirectX::XMMatrixScaling(transform->scale.x, transform->scale.y, transform->scale.z);
    return scl * rot * trans;
}

D3D11_PRIMITIVE_TOPOLOGY MeshRenderer::ParseTopology(const std::string &str) {
    if (str == "TRIANGLELIST") return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    if (str == "LINELIST") return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    if (str == "POINTLIST") return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
    return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}
