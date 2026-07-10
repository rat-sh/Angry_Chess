#pragma once
#include "engine/board/BoardState.hpp"
#include "engine/rules/Move.hpp"
#include <vector>

namespace angry_chess {

/// Win / draw condition checker for chess.
bge::GameStatus evaluateGameStatus(const bge::BoardState& state,
                                   bge::Color             activePlayer);

} // namespace angry_chess
