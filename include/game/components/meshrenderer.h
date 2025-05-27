#ifndef PROJECTANCHORPOINT_MESHRENDERER_H
#define PROJECTANCHORPOINT_MESHRENDERER_H

#include "game/components/component.h"
#include "game/components/transform.h"
#include "graphics/mesh.h"

class MeshRenderer : public Component {
    Mesh mesh;
    Transform transform;

    std::string getName() override;

    void Init(nlohmann::basic_json<> data) override;

    void Update() override;

    void Destroy() override;

    DirectX::XMMATRIX GetWorldMatrix() const;

    D3D11_PRIMITIVE_TOPOLOGY ParseTopology(const std::string &str);
};

REGISTER_COMPONENT(MeshRenderer);

#endif //PROJECTANCHORPOINT_MESHRENDERER_H
