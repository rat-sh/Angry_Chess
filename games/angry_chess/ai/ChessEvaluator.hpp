#pragma once
#include "engine/board/BoardState.hpp"

namespace angry_chess {

/// Material + positional evaluation for chess.
/// Positive = good for White, negative = good for Black.
/// Passed to MinimaxAI as an EvaluatorFn.
int chessEvaluate(const bge::BoardState& state);

} // namespace angry_chess
