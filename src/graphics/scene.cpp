#include "graphics/scene.h"

void Scene::Load(const std::string& sceneFile, ID3D11Device *device) {
    D3D11_BUFFER_DESC objDesc = {};
    objDesc.ByteWidth = sizeof(ObjectBuffer);
    objDesc.Usage = D3D11_USAGE_DYNAMIC;
    objDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    objDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    objDesc.StructureByteStride = 0;

    device->CreateBuffer(&objDesc, nullptr, &objectBuffer);
    std::ifstream in("scenes/" + sceneFile + ".scene");
    if (!in)
        return;

    json sceneData;
    in >> sceneData;

    for (const auto& obj : sceneData["objects"]) {
        if (obj["type"] == "mesh") {
            SceneObject newObject;
            Mesh mesh;
            mesh.LoadFromOBJ(obj["model"], ParseTopology(obj["topology"]));
            mesh.CreateVertexBuffer(device);
            newObject.mesh = mesh;

            auto tf = obj["transform"];
            newObject.position[0] = tf["position"][0];
            newObject.position[1] = tf["position"][1];
            newObject.position[2] = tf["position"][2];

            newObject.rotation[0] = tf["rotation"][0];
            newObject.rotation[1] = tf["rotation"][1];
            newObject.rotation[2] = tf["rotation"][2];

            newObject.scale[0] = tf["scale"][0];
            newObject.scale[1] = tf["scale"][1];
            newObject.scale[2] = tf["scale"][2];

            objects.push_back(std::move(newObject));
        } else if (obj["type"] == "light") {
            Light light;
            light.type = obj["lightType"];
            light.range = obj["range"];
            light.intensity = obj["intensity"];
            light.color[0] = obj["color"][0];
            light.color[1] = obj["color"][1];
            light.color[2] = obj["color"][2];

            auto tf = obj["transform"];
            light.position[0] = tf["position"][0];
            light.position[1] = tf["position"][1];
            light.position[2] = tf["position"][2];

            light.direction[0] = tf["rotation"][0];
            light.direction[1] = tf["rotation"][1];
            light.direction[2] = tf["rotation"][2];

            lights.push_back(std::move(light));
        }
    }

    D3D11_BUFFER_DESC lightDesc = {};
    lightDesc.Usage = D3D11_USAGE_DEFAULT;
    lightDesc.ByteWidth = sizeof(Light) * lights.size();
    lightDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    lightDesc.CPUAccessFlags = 0;
    lightDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    lightDesc.StructureByteStride = sizeof(Light);

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = lights.data();

    device->CreateBuffer(&lightDesc, &initData, &lightBuffer);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = lights.size();

   device->CreateShaderResourceView(lightBuffer, &srvDesc, &lightSRV);
}

void Scene::Draw(ID3D11DeviceContext *context) {
    for (auto& object : objects) {
        D3D11_MAPPED_SUBRESOURCE mapped = {};
        context->Map(objectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

        auto* buffer = reinterpret_cast<ObjectBuffer*>(mapped.pData);
        buffer->world = object.GetWorldMatrix();
        buffer->worldViewProj = object.GetWorldMatrix() * camera.GetViewMatrix() * camera.GetProjectionMatrix();

        context->Unmap(objectBuffer, 0);
        context->VSSetConstantBuffers(1, 1, &objectBuffer);
        object.mesh.Draw(context);
    }

    context->PSSetShaderResources(0, 1, &lightSRV);
}

void Scene::Unload() {
    objectBuffer->Release();
    for (auto& object : objects) {
        object.mesh.Release();
    }
}

D3D11_PRIMITIVE_TOPOLOGY Scene::ParseTopology(const std::string &str) {
    if (str == "TRIANGLELIST") return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    if (str == "LINELIST") return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    if (str == "POINTLIST") return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
    return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

DirectX::XMMATRIX SceneObject::GetWorldMatrix() const {
    DirectX::XMMATRIX trans = DirectX::XMMatrixTranslation(position[0], position[1], position[2]);
    DirectX::XMMATRIX rotX = DirectX::XMMatrixRotationX(rotation[0]);
    DirectX::XMMATRIX rotY = DirectX::XMMatrixRotationY(rotation[1]);
    DirectX::XMMATRIX rotZ = DirectX::XMMatrixRotationZ(rotation[2]);
    DirectX::XMMATRIX rot = rotZ * rotY * rotX;
    DirectX::XMMATRIX scl = DirectX::XMMatrixScaling(scale[0], scale[1], scale[2]);
    return scl * rot * trans;
}