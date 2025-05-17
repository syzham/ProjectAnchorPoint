#include "graphics/graphics.h"

IDXGISwapChain *sc = nullptr;
ID3D11Device *device = nullptr;
ID3D11DeviceContext *dc = nullptr;
ID3D11RenderTargetView *backBuffer = nullptr;

void InitD3D(HWND hWnd) {
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

    ID3D11Texture2D *pBackBuffer;
    sc->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *) &pBackBuffer);

    device->CreateRenderTargetView(pBackBuffer, nullptr, &backBuffer);
    pBackBuffer->Release();

    dc->OMSetRenderTargets(1, &backBuffer, nullptr);

    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = 800;
    viewport.Height = 600;

    dc->RSSetViewports(1, &viewport);
}

void RenderFrame() {
    float clearColour[4] = {0.2f, 0.4f, 0.6f, 1.0f};
    dc->ClearRenderTargetView(backBuffer, clearColour);
    sc->Present(0, 0);
}

void CleanD3D()
{
    sc->Release();
    backBuffer->Release();
    device->Release();
    dc->Release();
}
