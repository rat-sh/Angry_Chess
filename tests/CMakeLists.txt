cmake_minimum_required(VERSION 3.24)

# Discover Catch2 main
include(CTest)
include(Catch)

# ── Engine tests ──────────────────────────────────────────────────────────────
add_executable(test_engine
    engine/test_board_state.cpp
    engine/test_event_bus.cpp
)
target_include_directories(test_engine PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(test_engine PRIVATE engine Catch2::Catch2WithMain)
target_compile_features(test_engine PRIVATE cxx_std_20)
# No sanitizers on test exes: Catch2 is not instrumented
target_compile_options(test_engine PRIVATE -Wall -Wextra)
catch_discover_tests(test_engine)

# ── Chess plugin tests ─────────────────────────────────────────────────────────
add_executable(test_angry_chess
    angry_chess/test_chess_moves.cpp
    angry_chess/test_check_detection.cpp
    angry_chess/test_win_conditions.cpp
    angry_chess/test_fen.cpp
)
target_include_directories(test_angry_chess PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(test_angry_chess PRIVATE engine angry_chess Catch2::Catch2WithMain)
target_compile_features(test_angry_chess PRIVATE cxx_std_20)
target_compile_options(test_angry_chess PRIVATE -Wall -Wextra)
catch_discover_tests(test_angry_chess)
