cmake_minimum_required(VERSION 3.20)
project(AnchorPoint VERSION 0.2.0 LANGUAGES CXX)

option(ANCHORPOINT_BUILD_SHARED "Build AnchorPoint as a shared library (DLL)" ON)
option(ANCHORPOINT_BUILD_EXAMPLES "Build the example projects" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_compile_definitions(_DEBUG)
endif()

# ----------------------------------------------------------------------------
# Library
# ----------------------------------------------------------------------------

set(ANCHORPOINT_SOURCES
        src/core/engine.cpp
        src/core/log.cpp
        src/assets/mesh.cpp
        src/scene/scene_loader.cpp
        src/systems/collision_system.cpp
        src/systems/render_system.cpp
        src/platform/platform.cpp
)

if(WIN32)
    list(APPEND ANCHORPOINT_SOURCES
            src/platform/win32/win32_window.cpp
            src/platform/d3d11/d3d11_renderer.cpp
            src/platform/d3d11/WICTextureLoader11.cpp
    )
endif()

if(ANCHORPOINT_BUILD_SHARED)
    add_library(anchorpoint SHARED ${ANCHORPOINT_SOURCES})
    target_compile_definitions(anchorpoint
            PUBLIC ANCHORPOINT_SHARED
            PRIVATE ANCHORPOINT_EXPORTS)
else()
    add_library(anchorpoint STATIC ${ANCHORPOINT_SOURCES})
endif()
add_library(AnchorPoint::anchorpoint ALIAS anchorpoint)

target_include_directories(anchorpoint
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_target_properties(anchorpoint PROPERTIES
        VERSION ${PROJECT_VERSION}
        POSITION_INDEPENDENT_CODE ON
)

if(WIN32)
    target_compile_definitions(anchorpoint PRIVATE UNICODE _UNICODE NOMINMAX)
    target_link_libraries(anchorpoint PRIVATE d3d11 dxgi d3dcompiler user32)
endif()

if(MSVC)
    # Exported classes legitimately contain STL members; consumers must use
    # the same toolchain/runtime, which is standard for C++ SDKs.
    target_compile_options(anchorpoint PRIVATE /wd4251)
endif()

# ----------------------------------------------------------------------------
# Install rules: produces the include/, lib/ and bin/ (DLL) SDK layout and a
# CMake package so consumers can use find_package(AnchorPoint).
# ----------------------------------------------------------------------------

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS anchorpoint
        EXPORT AnchorPointTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT AnchorPointTargets
        NAMESPACE AnchorPoint::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/AnchorPoint
)

configure_package_config_file(
        cmake/AnchorPointConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/AnchorPointConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/AnchorPoint
)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/AnchorPointConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/AnchorPointConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/AnchorPointConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/AnchorPoint
)

# ----------------------------------------------------------------------------
# Examples
# ----------------------------------------------------------------------------

if(ANCHORPOINT_BUILD_EXAMPLES)
    add_subdirectory(examples/sandbox)
endif()
