#ifndef PROJECTANCHORPOINT_GRAPHICS_H
#define PROJECTANCHORPOINT_GRAPHICS_H

#include <d3d11.h>

#pragma comment (lib, "d3d11.lib")

extern IDXGISwapChain *sc;
extern ID3D11Device *device;
extern ID3D11DeviceContext *dc;

void InitD3D(HWND hWnd);
void CleanD3D();

#endif //PROJECTANCHORPOINT_GRAPHICS_H
