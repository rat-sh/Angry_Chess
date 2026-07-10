cmake_minimum_required(VERSION 3.24)

set(ENGINE_SOURCES
    # Core
    core/PluginRegistry.cpp
    core/GameEngine.cpp
    # AI
    ai/RandomAI.cpp
    # Board
    board/BoardState.cpp
    # Input
    input/ConsoleInputHandler.cpp
    # Renderer
    renderer/ConsoleRenderer.cpp
)

add_library(engine STATIC ${ENGINE_SOURCES})

target_include_directories(engine PUBLIC
    ${CMAKE_SOURCE_DIR}
)

target_compile_features(engine PUBLIC cxx_std_20)

target_compile_options(engine PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Debug>:-g -O0>
)
