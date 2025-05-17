#ifndef PROJECTANCHORPOINT_GRAPHICS_H
#define PROJECTANCHORPOINT_GRAPHICS_H

#include <d3d11.h>
#include "core/config.h"

#pragma comment (lib, "d3d11.lib")

extern IDXGISwapChain *sc;
extern ID3D11Device *device;
extern ID3D11DeviceContext *dc;
extern ID3D11RenderTargetView *backBuffer;

void InitD3D(HWND hWnd);
void RenderFrame();
void CleanD3D();

#endif //PROJECTANCHORPOINT_GRAPHICS_H
