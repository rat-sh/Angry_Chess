#pragma once
#include "engine/board/BoardState.hpp"
#include "engine/rules/Move.hpp"
#include <vector>
namespace angry_chess {
std::vector<bge::Move> bishopMoves(const bge::BoardState& state, bge::Position from);
/// Shared diagonal ray generator (used by Bishop and Queen)
void addDiagonalMoves(const bge::BoardState& state, bge::Position from,
                      bge::Color color, std::vector<bge::Move>& out);
}
