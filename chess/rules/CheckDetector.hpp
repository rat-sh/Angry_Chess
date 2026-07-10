#pragma once
#include "engine/board/BoardState.hpp"

namespace angry_chess {

/// Determines whether `color`'s king is under attack in `state`.
/// Checks all opposing piece move generators for attacks on the king square.
bool isInCheck(const bge::BoardState& state, bge::Color color);

/// Returns true if `square` is attacked by any piece of `attackerColor`.
bool isSquareAttacked(const bge::BoardState& state,
                      bge::Position          square,
                      bge::Color             attackerColor);

} // namespace angry_chess
