add_executable(sandbox
        main.cpp
        game/game_state.cpp
        components/register.cpp
)
# Sandbox sources include each other relative to this directory
# (e.g. #include "systems/spin_system.h").
target_include_directories(sandbox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sandbox PRIVATE AnchorPoint::anchorpoint)

# Run the sandbox from its own build directory with the data it needs.
add_custom_command(TARGET sandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/scenes $<TARGET_FILE_DIR:sandbox>/scenes
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:sandbox>/assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:sandbox>/shaders)

if(ANCHORPOINT_BUILD_SHARED)
    add_custom_command(TARGET sandbox POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:anchorpoint> $<TARGET_FILE_DIR:sandbox>)
endif()
