#include "graphics/scene.h"

void Scene::Load(const std::string& sceneFile) {

    std::ifstream in("scenes/" + sceneFile + ".scene");
    if (!in)
        return;

    json sceneData;
    in >> sceneData;

    for (const auto& obj : sceneData["objects"]) {
        objects.push_back(std::make_unique<SceneObject>());
        SceneObject* newObject = objects.back().get();
        for (const auto& component : obj.items()) {
            if (component.key() == "Name") {
                newObject->name = component.value();
                continue;
            }

            std::unique_ptr<Component> newComponent = ComponentFactory::Instance().Create(component.key());
            if (newComponent) {
                newComponent->owner = newObject;
                newComponent->Init(component.value());
                newObject->components.push_back(std::move(newComponent));
            }
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

void Scene::Draw() {
    for (auto& object : objects) {
        for (auto& comp : object->components) {
            comp->Update();
        }
    }

    dc->PSSetShaderResources(0, 1, &lightSRV);
}

void Scene::Unload() {
    for (auto& object : objects) {
        for (auto& comp : object->components) {
            comp->Destroy();
        }
    }
}
