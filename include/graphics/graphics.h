#ifndef PROJECTANCHORPOINT_GRAPHICS_H
#define PROJECTANCHORPOINT_GRAPHICS_H

#include <d3d11.h>
#include "core/config.h"
#include "graphics/shader.h"
#include <DirectXMath.h>
#include "core/logger.h"

#pragma comment (lib, "d3d11.lib")

struct ObjectBuffer {
    DirectX::XMFLOAT4X4 worldViewProj;
    DirectX::XMFLOAT4X4 world;
};

extern IDXGISwapChain *sc;
extern ID3D11Device *device;
extern ID3D11DeviceContext *dc;
extern ID3D11RenderTargetView *backBuffer;
extern ID3D11DepthStencilView *depthView;
extern ID3D11DepthStencilState *depthState;
extern ID3D11Buffer *objectBuffer;

void InitD3D(HWND hWnd);
void InitGraphics();
void ClearFrame();
void RenderFrame();
void CleanD3D();

#endif //PROJECTANCHORPOINT_GRAPHICS_H
