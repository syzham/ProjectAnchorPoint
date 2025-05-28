#include "graphics/light.h"

ID3D11Buffer* Lights::lightBuffer = nullptr;
ID3D11ShaderResourceView* Lights::lightSRV = nullptr;
std::vector<Light> Lights::activeLights;

void Lights::Init() {
    D3D11_BUFFER_DESC lightDesc = {};
    lightDesc.Usage = D3D11_USAGE_DEFAULT;
    lightDesc.ByteWidth = sizeof(Light) * Lights::activeLights.size();
    lightDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    lightDesc.CPUAccessFlags = 0;
    lightDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    lightDesc.StructureByteStride = sizeof(Light);

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = Lights::activeLights.data();

    device->CreateBuffer(&lightDesc, &initData, &Lights::lightBuffer);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = activeLights.size();

    device->CreateShaderResourceView(Lights::lightBuffer, &srvDesc, &Lights::lightSRV);
}

void Lights::Update() {
    dc->PSSetShaderResources(0, 1, &Lights::lightSRV);
}

void Lights::Release() {
    Lights::lightBuffer->Release();
    Lights::lightSRV->Release();
}