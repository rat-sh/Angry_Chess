#pragma once
#include "engine/board/BoardState.hpp"
#include "engine/rules/Move.hpp"
#include <vector>
namespace angry_chess {
/// Basic 1-square king moves (does NOT include castling — CastlingRule adds those).
std::vector<bge::Move> kingMoves(const bge::BoardState& state, bge::Position from);
}
