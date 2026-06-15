#ifndef ANCHORPOINT_PLATFORM_D3D11_RENDERER_H
#define ANCHORPOINT_PLATFORM_D3D11_RENDERER_H

#include <d3d11.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "anchorpoint/platform/renderer.h"

namespace ap {

// Direct3D 11 rendering backend (Windows only).
class D3D11Renderer final : public Renderer {
public:
    bool Init(Window& window, const EngineConfig& config) override;
    MeshHandle CreateMesh(const MeshData& data) override;
    void RenderFrame(const FrameData& frame) override;
    void Shutdown() override;

private:
    struct ShaderProgram {
        ID3D11VertexShader* vertexShader = nullptr;
        ID3D11PixelShader* pixelShader = nullptr;
        ID3D11InputLayout* inputLayout = nullptr;
    };

    struct MeshResource {
        ID3D11Buffer* vertexBuffer = nullptr;
        ID3D11Buffer* materialBuffer = nullptr;
        ID3D11ShaderResourceView* textureSRV = nullptr;
        ID3D11SamplerState* samplerState = nullptr;
        ShaderProgram shader;
        UINT vertexCount = 0;
        D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        float diffuseColor[4] = {1, 1, 1, 1};
    };

    bool LoadShader(const std::wstring& vsPath, const std::wstring& psPath, ShaderProgram& out);
    bool LoadDebugShader();
    void ReleaseShader(ShaderProgram& shader);
    void ReleaseMesh(MeshResource& mesh);
    void UpdateLightBuffer(const std::vector<GpuLight>& lights);
    void DrawDebugLines(const std::vector<DebugVertex>& lines, const Matrix4& viewProj);

    IDXGISwapChain* swapChain = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    ID3D11RenderTargetView* backBuffer = nullptr;
    ID3D11DepthStencilView* depthView = nullptr;
    ID3D11DepthStencilState* depthState = nullptr;
    ID3D11Buffer* objectBuffer = nullptr;

    ID3D11Buffer* lightBuffer = nullptr;
    ID3D11ShaderResourceView* lightSRV = nullptr;
    std::size_t lightCapacity = 0;

    ShaderProgram debugShader;
    ID3D11Buffer* debugVertexBuffer = nullptr;
    std::size_t debugVertexCapacity = 0;

    std::unordered_map<MeshHandle, MeshResource> meshes;
    MeshHandle nextHandle = 1;
};

} // namespace ap

#endif //ANCHORPOINT_PLATFORM_D3D11_RENDERER_H
