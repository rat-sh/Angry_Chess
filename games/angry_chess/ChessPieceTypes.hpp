#pragma once
#include "engine/core/Types.hpp"

/// Chess piece type IDs registered by AngryChessPlugin.
/// These are the values stored in BoardState::grid cells.
namespace angry_chess {

constexpr bge::PieceTypeID PAWN   = 1;
constexpr bge::PieceTypeID KNIGHT = 2;
constexpr bge::PieceTypeID BISHOP = 3;
constexpr bge::PieceTypeID ROOK   = 4;
constexpr bge::PieceTypeID QUEEN  = 5;
constexpr bge::PieceTypeID KING   = 6;

/// Helper: is pos inside the board?
inline bool inBounds(int r, int f) noexcept {
    return r >= 0 && r < 8 && f >= 0 && f < 8;
}

} // namespace angry_chess
