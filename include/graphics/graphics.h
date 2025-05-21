#ifndef PROJECTANCHORPOINT_GRAPHICS_H
#define PROJECTANCHORPOINT_GRAPHICS_H

#include <d3d11.h>
#include "core/config.h"
#include "graphics/scene.h"
#include "graphics/shader.h"

#pragma comment (lib, "d3d11.lib")

extern IDXGISwapChain *sc;
extern ID3D11Device *device;
extern ID3D11DeviceContext *dc;
extern ID3D11RenderTargetView *backBuffer;
extern ID3D11DepthStencilView *depthView;
extern ID3D11DepthStencilState *depthState;
extern Scene g_scene;
extern Shader g_shader;

void InitD3D(HWND hWnd);
void InitGraphics(const std::string& sceneFile);
void RenderFrame();
void CleanD3D();

#endif //PROJECTANCHORPOINT_GRAPHICS_H
