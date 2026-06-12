#include "core/gameEngine.h"

int GameEngine::Init(HINSTANCE hInstance, int nCmdShow) {
    Log("Initializing game engine...");
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

    LARGE_INTEGER frequency{};
    QueryPerformanceFrequency(&frequency);
    LARGE_INTEGER lastFrame{};
    QueryPerformanceCounter(&lastFrame);
    double accum     = 0.0;
    double deltaTime = 0.0;
    constexpr double fixedDelta = 1.0 / 60.0;

    while (true) {
        MSG msg = {};
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (msg.message == WM_QUIT) break;

        LARGE_INTEGER currFrame{};
        QueryPerformanceCounter(&currFrame);
        deltaTime = static_cast<double>(currFrame.QuadPart - lastFrame.QuadPart)
                  / static_cast<double>(frequency.QuadPart);
        if (deltaTime > 0.25) deltaTime = 0.25;
        accum += deltaTime;

        ClearFrame();

        while (accum >= fixedDelta) {
            FixedUpdateScript();
            accum -= fixedDelta;
        }

        // ScriptSystem calls UpdateScript internally; collision, camera, lights, and
        // render systems run through SceneManager::update.
        SceneManager::getInstance().update(static_cast<float>(deltaTime));

        lastFrame = currFrame;
        RenderFrame();
    }
}

void GameEngine::Shutdown() {
    CleanD3D();
    CloseHost();
    SceneManager::getInstance().unloadScene();
}
