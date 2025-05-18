#ifndef PROJECTANCHORPOINT_SCENE_H
#define PROJECTANCHORPOINT_SCENE_H

#include <vector>
#include "graphics/mesh.h"

class Scene {
public:
    std::vector<Mesh> meshes;

    void Load(const std::string& sceneFile, ID3D11Device* device);
    void Draw(ID3D11DeviceContext* context);
    void Unload();

private:
    D3D11_PRIMITIVE_TOPOLOGY ParseTopology(const std::string& str);
};

#endif //PROJECTANCHORPOINT_SCENE_H
