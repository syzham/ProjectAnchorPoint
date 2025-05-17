cmake_minimum_required(VERSION 3.31)
project(ProjectAnchorPoint)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(ProjectAnchorPoint
        include/window/window.h
        src/window/window.cpp
        include/graphics/graphics.h
        src/graphics/graphics.cpp
        include/core/config.h
        src/core/config.cpp)

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
