cmake_minimum_required(VERSION 3.31)
project(ProjectAnchorPoint)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_compile_definitions(_DEBUG)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(ProjectAnchorPoint
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
)

target_link_libraries(
        ProjectAnchorPoint
        d3d11
        dxgi
        d3dcompiler
        user32
)
target_compile_definitions(ProjectAnchorPoint PRIVATE UNICODE _UNICODE)
set_target_properties(ProjectAnchorPoint PROPERTIES WIN32_EXECUTABLE ON)

include_directories(
        "C:/Program Files (x86)/Windows Kits/10/Include/10.0.xxxxx.x/um"
        ${PROJECT_SOURCE_DIR}/include
)
link_directories("C:/Program Files (x86)/Windows Kits/10/Lib/10.0.xxxxx.x/um/x64")

add_custom_command(TARGET ProjectAnchorPoint POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/scenes $<TARGET_FILE_DIR:ProjectAnchorPoint>/scenes)

add_custom_command(TARGET ProjectAnchorPoint POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:ProjectAnchorPoint>/assets)

add_custom_command(TARGET ProjectAnchorPoint POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:ProjectAnchorPoint>/shaders)