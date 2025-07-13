#include <Windows.h>
#include "core/gameEngine.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#ifdef _DEBUG
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif
    LogHeader("Project Anchor Point");

    GameEngine engine;
    if (engine.Init(hInstance, nCmdShow) != 0) {
        LogError("Failed to initialize game engine");
        return -1;
    }

    engine.Run();

    engine.Shutdown();
    return 0;
}