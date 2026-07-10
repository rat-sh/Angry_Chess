#pragma once
#include "engine/board/BoardState.hpp"
#include "engine/rules/Move.hpp"
#include <vector>
namespace angry_chess {
/// Appends castling moves (kingside / queenside) for `color` if conditions met:
///   - King not in check
///   - Castling rights still available
///   - Intervening squares empty
///   - King does not pass through attacked square
void addCastlingMoves(const bge::BoardState& state,
                      bge::Color             color,
                      std::vector<bge::Move>& out);
}
