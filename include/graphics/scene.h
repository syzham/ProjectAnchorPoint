#ifndef PROJECTANCHORPOINT_SCENE_H
#define PROJECTANCHORPOINT_SCENE_H

#include <vector>
#include "graphics/camera.h"
#include "graphics/light.h"
#include "graphics/mesh.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct ObjectBuffer {
    DirectX::XMMATRIX worldViewProj;
    DirectX::XMMATRIX world;
};

class SceneObject {
public:
    float position[3] = {0, 0, 0};
    float rotation[3] = {0, 0, 0};
    float scale[3] = {0, 0, 0};

    Mesh mesh;

    DirectX::XMMATRIX GetWorldMatrix() const;
};

class Scene {
public:
    std::vector<SceneObject> objects;
    std::vector<Light> lights;
    ID3D11Buffer* objectBuffer = nullptr;
    ID3D11Buffer* lightBuffer = nullptr;
    ID3D11ShaderResourceView* lightSRV = nullptr;
    Camera camera;

    void Load(const std::string& sceneFile, ID3D11Device* device);
    void Draw(ID3D11DeviceContext* context);
    void Unload();

private:
    D3D11_PRIMITIVE_TOPOLOGY ParseTopology(const std::string& str);
};

#endif //PROJECTANCHORPOINT_SCENE_H
