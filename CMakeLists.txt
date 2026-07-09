cmake_minimum_required(VERSION 3.24)
project(AngryChessEngine VERSION 1.0.0 LANGUAGES CXX)

# ── Global settings ────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ── Dependencies via FetchContent ──────────────────────────────────────────────
include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.3
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(Catch2)

# ── Subdirectories ─────────────────────────────────────────────────────────────
add_subdirectory(engine)
add_subdirectory(games/angry_chess)

# ── Application executable ─────────────────────────────────────────────────────
add_executable(angry_chess_game app/main.cpp)

target_include_directories(angry_chess_game PRIVATE
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(angry_chess_game PRIVATE
    engine
    angry_chess
)

target_compile_features(angry_chess_game PRIVATE cxx_std_20)

target_compile_options(angry_chess_game PRIVATE
    -Wall -Wextra -Wpedantic
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Debug>:-g -O0 -fsanitize=address,undefined>
)

target_link_options(angry_chess_game PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)

# ── Tests ──────────────────────────────────────────────────────────────────────
enable_testing()
add_subdirectory(tests)

# ── SDL2 Graphical target (optional) ───────────────────────────────────────────
find_package(SDL2 QUIET)
find_package(SDL2_ttf QUIET)

if(SDL2_FOUND AND SDL2_ttf_FOUND)
    message(STATUS " SDL2 found — building angry_chess_sdl")

    add_executable(angry_chess_sdl
        app/sdl_main.cpp
        engine/renderer/SDL2Renderer.cpp
        engine/input/SDL2InputHandler.cpp
    )

    target_include_directories(angry_chess_sdl PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${SDL2_INCLUDE_DIRS}
    )

    target_link_libraries(angry_chess_sdl PRIVATE
        engine
        angry_chess
        SDL2::SDL2
        SDL2_ttf::SDL2_ttf
    )

    target_compile_features(angry_chess_sdl PRIVATE cxx_std_20)
    target_compile_options(angry_chess_sdl PRIVATE
        -Wall -Wextra
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Debug>:-g -O0>
    )

else()
    message(STATUS " SDL2 not found — skipping angry_chess_sdl (run: sudo apt-get install libsdl2-dev libsdl2-ttf-dev)")
endif()

# ── Summary ────────────────────────────────────────────────────────────────────
message(STATUS "-------------------------------------------")
message(STATUS " Angry Chess Engine  v${PROJECT_VERSION}")
message(STATUS " Build type  : ${CMAKE_BUILD_TYPE}")
message(STATUS " C++ standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS " Source dir  : ${CMAKE_SOURCE_DIR}")
message(STATUS " Binary dir  : ${CMAKE_BINARY_DIR}")
message(STATUS "-------------------------------------------")
