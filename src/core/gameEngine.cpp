#include "core/gameEngine.h"

int GameEngine::Init(HINSTANCE hInstance, int nCmdShow) {
    window.Create(hInstance, nCmdShow, L"Project Anchor Point");
    cursor = Cursor(window.getHwnd());
    InitD3D(window.getHwnd());
    InitHost();
    SceneManager::getInstance().loadFirstScene();
    return 0;
}

void GameEngine::Run() {
    window.Show();
    cursor.Centre();
    cursor.Hide();
    while (true) {
        MSG msg = {};
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (msg.message == WM_QUIT)
            break;

        ClearFrame();


        UpdateScript();
        SceneManager::getInstance().update();


        RenderFrame();
    }
}

void GameEngine::Shutdown() {
    CleanD3D();
    CloseHost();
    SceneManager::getInstance().unloadScene();
}