cmake_minimum_required(VERSION 3.24)

set(ANGRY_CHESS_SOURCES
    pieces/Pawn.cpp
    pieces/Knight.cpp
    pieces/Bishop.cpp
    pieces/Rook.cpp
    pieces/Queen.cpp
    pieces/King.cpp
    rules/CheckDetector.cpp
    rules/CastlingRule.cpp
    rules/ChessWinCondition.cpp
    rules/ChessRules.cpp
    ai/ChessEvaluator.cpp
    FenParser.cpp
    AngryChessPlugin.cpp
)

add_library(angry_chess STATIC ${ANGRY_CHESS_SOURCES})

target_include_directories(angry_chess PUBLIC
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(angry_chess PUBLIC engine)

target_compile_features(angry_chess PUBLIC cxx_std_20)

target_compile_options(angry_chess PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Debug>:-g -O0>
)
