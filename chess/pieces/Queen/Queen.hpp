#pragma once
#include "engine/board/BoardState.hpp"
#include "engine/rules/Move.hpp"
#include <vector>
namespace angry_chess {
std::vector<bge::Move> queenMoves(const bge::BoardState& state, bge::Position from);
}
