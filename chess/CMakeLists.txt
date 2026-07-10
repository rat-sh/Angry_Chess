cmake_minimum_required(VERSION 3.24)

set(ANGRY_CHESS_SOURCES
    # Pieces — each piece owns its own folder
    pieces/Pawn/Pawn.cpp
    pieces/Knight/Knight.cpp
    pieces/Bishop/Bishop.cpp
    pieces/Rook/Rook.cpp
    pieces/Queen/Queen.cpp
    pieces/King/King.cpp
    # Rules — movement rules
    rules/movement/CastlingRule.cpp
    # Rules — win conditions
    rules/win/ChessWinCondition.cpp
    # Rules — core orchestrator + check detection
    rules/CheckDetector.cpp
    rules/ChessRules.cpp
    # AI — Stockfish UCI bridge
    ai/StockfishAI/StockfishAI.cpp
    # Shared utilities
    notation/Fen/FenParser.cpp
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
