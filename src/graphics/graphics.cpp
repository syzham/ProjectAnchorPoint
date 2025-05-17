#include "graphics/graphics.h"

IDXGISwapChain *sc = nullptr;
ID3D11Device *device = nullptr;
ID3D11DeviceContext *dc = nullptr;

void InitD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC scd;

    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hWnd;
    scd.SampleDesc.Count = 4;
    scd.Windowed = TRUE;

    D3D11CreateDeviceAndSwapChain(nullptr,
                                  D3D_DRIVER_TYPE_HARDWARE,
                                  nullptr,
                                  NULL,
                                  nullptr,
                                  NULL,
                                  D3D11_SDK_VERSION,
                                  &scd,
                                  &sc,
                                  &device,
                                  nullptr,
                                  &dc);
}

void CleanD3D()
{
    sc->Release();
    device->Release();
    dc->Release();
}
