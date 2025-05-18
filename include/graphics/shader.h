#ifndef PROJECTANCHORPOINT_SHADER_H
#define PROJECTANCHORPOINT_SHADER_H

#include <d3d11.h>
#include <string>
#include <d3dcompiler.h>
#include <fstream>

#pragma comment(lib, "d3dcompiler.lib")

class Shader {
public:
    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11InputLayout* inputLayout = nullptr;

    bool Load(ID3D11Device* device, const std::wstring& vsPath, const std::wstring& psPath);
    void Bind(ID3D11DeviceContext* context);
    void Release();
};

#endif //PROJECTANCHORPOINT_SHADER_H
