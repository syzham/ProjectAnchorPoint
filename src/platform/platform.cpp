#include "anchorpoint/platform/renderer.h"
#include "anchorpoint/platform/window.h"

#include "null/null_platform.h"

#if defined(_WIN32)
#include "win32/win32_window.h"
#include "d3d11/d3d11_renderer.h"
#endif

namespace ap {

std::unique_ptr<Window> CreatePlatformWindow(const EngineConfig& config) {
#if defined(_WIN32)
    if (!config.headless)
        return std::make_unique<Win32Window>();
#endif
    return std::make_unique<NullWindow>();
}

std::unique_ptr<Renderer> CreatePlatformRenderer(const EngineConfig& config) {
#if defined(_WIN32)
    if (!config.headless)
        return std::make_unique<D3D11Renderer>();
#endif
    return std::make_unique<NullRenderer>();
}

} // namespace ap
