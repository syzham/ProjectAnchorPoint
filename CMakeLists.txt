cmake_minimum_required(VERSION 3.31)
project(ProjectAnchorPoint)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_compile_definitions(_DEBUG)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ──────────────────────────────────────────────
# Engine library (DLL)
# ──────────────────────────────────────────────
add_library(ProjectAnchorPoint SHARED
        # window
        include/window/window.h
        src/window/window.cpp
        include/window/cursor.h
        src/window/cursor.cpp

        # graphics
        include/graphics/graphics.h
        src/graphics/graphics.cpp
        include/graphics/mesh.h
        src/graphics/mesh.cpp
        include/graphics/scene.h
        src/graphics/scene.cpp
        include/graphics/shader.h
        src/graphics/shader.cpp
        include/graphics/camera.h
        src/graphics/camera.cpp
        include/graphics/light.h
        src/graphics/light.cpp
        src/WICTextureLoader11.cpp

        # ECS core
        include/core/Registry.h
        include/core/System.h
        include/core/gameEngine.h
        src/core/gameEngine.cpp
        include/core/config.h
        src/core/config.cpp
        include/core/logger.h
        src/core/logger.cpp

        # ECS components (data structs – no .cpp needed except for method impls)
        include/common.h
        include/game/components/transform.h
        src/game/components/transform.cpp
        include/game/components/meshrenderer.h
        src/game/components/meshrenderer.cpp
        include/game/components/lightcomponent.h
        include/game/components/cameracomponent.h
        include/game/components/scriptcomponent.h
        include/game/components/colliders/aacollider.h

        # ECS systems
        include/systems/RenderSystem.h
        src/systems/RenderSystem.cpp
        include/systems/CameraSystem.h
        src/systems/CameraSystem.cpp
        include/systems/LightSystem.h
        src/systems/LightSystem.cpp
        include/systems/CollisionSystem.h
        src/systems/CollisionSystem.cpp
        include/systems/ScriptSystem.h
        src/systems/ScriptSystem.cpp

        # game managers
        include/game/SceneManager.h
        src/game/SceneManager.cpp
        include/game/InputManager.h
        src/game/InputManager.cpp

        # .NET scripting host
        include/game/ScriptBehaviour.h
        src/game/ScriptBehaviour.cpp

        # public umbrella header
        include/anchorpoint.h
)

# Export macro: PROJECTANCHORPOINT_EXPORTS is defined when building the DLL;
# consumers get __declspec(dllimport) automatically via the generated header.
target_compile_definitions(ProjectAnchorPoint
        PRIVATE PROJECTANCHORPOINT_EXPORTS UNICODE _UNICODE)

target_include_directories(ProjectAnchorPoint PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/external/nethost/include
        "C:/Program Files (x86)/Windows Kits/10/Include/10.0.xxxxx.x/um"
)

target_link_directories(ProjectAnchorPoint PRIVATE
        "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.xxxxx.x/um/x64"
        ${PROJECT_SOURCE_DIR}/external/nethost/lib
)

target_link_libraries(ProjectAnchorPoint PRIVATE
        d3d11
        dxgi
        d3dcompiler
        user32
        ${PROJECT_SOURCE_DIR}/external/nethost/lib/nethost.lib
)

# ──────────────────────────────────────────────
# ScriptExport DLL (C API used by C# scripts)
# ──────────────────────────────────────────────
add_library(ScriptExport SHARED
        include/game/ScriptExport.h
        src/game/ScriptExport.cpp
        include/game/components/transform.h
        src/game/components/transform.cpp
        include/game/InputManager.h
        src/game/InputManager.cpp
        include/core/config.h
        src/core/config.cpp
)

target_include_directories(ScriptExport PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        "C:/Program Files (x86)/Windows Kits/10/Include/10.0.xxxxx.x/um"
)

# ──────────────────────────────────────────────
# Sample application (links the engine DLL)
# ──────────────────────────────────────────────
add_executable(ProjectAnchorPointSample src/main.cpp)
target_compile_definitions(ProjectAnchorPointSample PRIVATE UNICODE _UNICODE)
set_target_properties(ProjectAnchorPointSample PROPERTIES WIN32_EXECUTABLE ON)
target_link_libraries(ProjectAnchorPointSample PRIVATE ProjectAnchorPoint)

# ──────────────────────────────────────────────
# Runtime assets / DLLs copied next to the sample
# ──────────────────────────────────────────────
set(RUNTIME_DLLS
        ${CMAKE_SOURCE_DIR}/hostfxr.dll
        ${CMAKE_SOURCE_DIR}/nethost.dll
)
set(RUNTIME_CONFIGS
        ${CMAKE_SOURCE_DIR}/GameScripts.runtimeconfig.json
)

foreach(TARGET_NAME ProjectAnchorPointSample)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/scenes $<TARGET_FILE_DIR:${TARGET_NAME}>/scenes)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${TARGET_NAME}>/assets)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:${TARGET_NAME}>/shaders)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${RUNTIME_DLLS} ${RUNTIME_CONFIGS}
            $<TARGET_FILE_DIR:${TARGET_NAME}>)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/scripts/GameScripts/bin/Release/net8.0
            $<TARGET_FILE_DIR:${TARGET_NAME}>)
endforeach()

add_custom_target(BuildGameScriptsDLL
    COMMAND dotnet build ${CMAKE_SOURCE_DIR}/scripts/GameScripts -c Release -p:Platform=""
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/scripts/GameScripts/bin/Release/net8.0/GameScripts.dll
        ${CMAKE_SOURCE_DIR}/GameScripts.dll
    BYPRODUCTS ${CMAKE_SOURCE_DIR}/GameScripts.dll
    COMMENT "Building and copying GameScripts.dll"
    VERBATIM
)

add_dependencies(ProjectAnchorPointSample ProjectAnchorPoint ScriptExport BuildGameScriptsDLL)

# ──────────────────────────────────────────────
# Install targets (DLL + import lib + headers)
# ──────────────────────────────────────────────
install(TARGETS ProjectAnchorPoint ScriptExport
    RUNTIME DESTINATION bin       # .dll
    ARCHIVE DESTINATION lib       # .lib (import library)
    LIBRARY DESTINATION lib
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
