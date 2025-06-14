cmake_minimum_required(VERSION 3.31)
project(ProjectAnchorPoint)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_compile_definitions(_DEBUG)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(ProjectAnchorPoint
        src/main.cpp
        include/window/window.h
        src/window/window.cpp
        include/graphics/graphics.h
        src/graphics/graphics.cpp
        include/core/config.h
        src/core/config.cpp
        include/graphics/mesh.h
        src/graphics/mesh.cpp
        include/graphics/scene.h
        src/graphics/scene.cpp
        include/graphics/shader.h
        src/graphics/shader.cpp
        include/graphics/camera.h
        src/graphics/camera.cpp
        include/graphics/light.h
        src/WICTextureLoader11.cpp
        include/core/gameEngine.h
        include/game/SceneManager.h
        src/game/SceneManager.cpp
        src/core/gameEngine.cpp
        include/game/components/component.h
        src/game/components/transform.cpp
        include/game/components/transform.h
        include/game/components/meshrenderer.h
        src/game/components/meshrenderer.cpp
        include/game/components/lightcomponent.h
        src/game/components/lightcomponent.cpp
        src/graphics/light.cpp
        src/game/ScriptBehaviour.cpp
        include/game/ScriptBehaviour.h
        include/game/components/scriptcomponent.h
        src/game/components/scriptcomponent.cpp
        include/window/cursor.h
        src/window/cursor.cpp
        include/game/components/cameracomponent.h
        src/game/components/cameracomponent.cpp
        include/game/InputManager.h
        src/game/InputManager.cpp
)

set(
        DLLS
        ${CMAKE_SOURCE_DIR}/hostfxr.dll
        ${CMAKE_SOURCE_DIR}/nethost.dll
)

set(
        CONFIGS
        ${CMAKE_SOURCE_DIR}/GameScripts.runtimeconfig.json
)

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

target_compile_definitions(ProjectAnchorPoint PRIVATE UNICODE _UNICODE)
set_target_properties(ProjectAnchorPoint PROPERTIES WIN32_EXECUTABLE ON)

include_directories(
        "C:/Program Files (x86)/Windows Kits/10/Include/10.0.xxxxx.x/um"
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/external/nethost/include
)
link_directories(
        "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.xxxxx.x/um/x64"
        ${PROJECT_SOURCE_DIR}/external/nethost/lib
)

target_link_libraries(
        ProjectAnchorPoint
        d3d11
        dxgi
        d3dcompiler
        user32
        ${PROJECT_SOURCE_DIR}/external/nethost/lib/nethost.lib
)


add_custom_command(TARGET ProjectAnchorPoint POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/scenes $<TARGET_FILE_DIR:ProjectAnchorPoint>/scenes)

add_custom_command(TARGET ProjectAnchorPoint POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:ProjectAnchorPoint>/assets)

add_custom_command(TARGET ProjectAnchorPoint POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:ProjectAnchorPoint>/shaders)

add_custom_command(TARGET ProjectAnchorPoint POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${DLLS}
        ${CONFIGS}
        $<TARGET_FILE_DIR:ProjectAnchorPoint>)

add_custom_target(BuildGameScriptsDLL
        COMMAND dotnet build ${CMAKE_SOURCE_DIR}/scripts/GameScripts -c Release -p:Platform=""
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/scripts/GameScripts/bin/Release/net8.0/GameScripts.dll
        ${CMAKE_SOURCE_DIR}/GameScripts.dll
        BYPRODUCTS ${CMAKE_SOURCE_DIR}/GameScripts.dll
        COMMENT "Building and copying GameScripts.dll"
        VERBATIM
)

add_custom_command(TARGET ProjectAnchorPoint POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/scripts/GameScripts/bin/Release/net8.0 $<TARGET_FILE_DIR:ProjectAnchorPoint>)

add_dependencies(ProjectAnchorPoint ScriptExport BuildGameScriptsDLL)
