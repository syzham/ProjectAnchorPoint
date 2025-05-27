#include "graphics/graphics.h"

IDXGISwapChain *sc = nullptr;
ID3D11Device *device = nullptr;
ID3D11DeviceContext *dc = nullptr;
ID3D11RenderTargetView *backBuffer = nullptr;
ID3D11DepthStencilView *depthView = nullptr;
ID3D11DepthStencilState *depthState = nullptr;
ID3D11Buffer *objectBuffer = nullptr;
Shader g_shader;

void InitD3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC scd;

    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferDesc.Height = screenHeight;
    scd.BufferDesc.Width = screenWidth;
    scd.OutputWindow = hWnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

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
    viewport.Width = (float)screenWidth;
    viewport.Height = (float)screenHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    dc->RSSetViewports(1, &viewport);

    InitGraphics();
}

void InitGraphics() {

    D3D11_BUFFER_DESC objDesc = {};
    objDesc.ByteWidth = sizeof(ObjectBuffer);
    objDesc.Usage = D3D11_USAGE_DYNAMIC;
    objDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    objDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    objDesc.StructureByteStride = 0;

    device->CreateBuffer(&objDesc, nullptr,&objectBuffer);

    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = screenWidth;
    depthDesc.Height = screenHeight;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthDesc.CPUAccessFlags = 0;
    depthDesc.MiscFlags = 0;

    ID3D11Texture2D *depthTexture;
    device->CreateTexture2D(&depthDesc, nullptr, &depthTexture);

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = depthDesc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    device->CreateDepthStencilView(depthTexture, nullptr, &depthView);
    depthTexture->Release();


    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    device->CreateDepthStencilState(&dsDesc, &depthState);

    g_shader.Load(device, L"shaders/VS.hlsl", L"shaders/PS.hlsl");
}

void ClearFrame() {
    float clearColour[4] = {0.2f, 0.4f, 0.6f, 1.0f};
    dc->ClearRenderTargetView(backBuffer, clearColour);
    dc->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &backBuffer, depthView);
    dc->OMSetDepthStencilState(depthState, 0);

    g_shader.Bind(dc);
}

void RenderFrame() {
    sc->Present(0, 0);
}

void CleanD3D()
{
    g_shader.Release();
    depthView->Release();
    depthState->Release();
    sc->SetFullscreenState(FALSE, nullptr);
    sc->Release();
    backBuffer->Release();
    device->Release();
    dc->Release();
}
