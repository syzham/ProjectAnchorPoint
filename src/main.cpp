#include <Windows.h>
#include "core/gameEngine.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#ifdef _DEBUG
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif
    GameEngine engine;
    if (engine.Init(hInstance, nCmdShow) != 0) {
        std::cout << "Failed to initialize game engine" << std::endl;
        return -1;
    }

    engine.Run();

    engine.Shutdown();
    return 0;
}