#pragma once
#include "engine/board/BoardState.hpp"
#include "engine/rules/Move.hpp"
#include <vector>

namespace angry_chess {

/// Generate pseudo-legal moves for a Pawn at `from`.
/// Handles single push, double push (first move), and diagonal captures.
/// En-passant and full flag annotation are handled in EnPassantRule.
std::vector<bge::Move> pawnMoves(const bge::BoardState& state, bge::Position from);

} // namespace angry_chess
