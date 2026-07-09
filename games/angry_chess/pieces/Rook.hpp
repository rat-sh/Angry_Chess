#pragma once
#include "engine/board/BoardState.hpp"
#include "engine/rules/Move.hpp"
#include <vector>
namespace angry_chess {
std::vector<bge::Move> rookMoves(const bge::BoardState& state, bge::Position from);
/// Shared orthogonal ray generator (used by Rook and Queen)
void addOrthogonalMoves(const bge::BoardState& state, bge::Position from,
                        bge::Color color, std::vector<bge::Move>& out);
}
