#pragma once
#include "engine/board/BoardState.hpp"

namespace bge {

/// Determines win, draw, or ongoing status.
/// Separated from IGameRules for composability and single-responsibility.
class IWinCondition {
public:
    virtual ~IWinCondition() = default;

    [[nodiscard]] virtual GameStatus evaluate(
        const BoardState& state,
        Color             activePlayer) const = 0;
};

} // namespace bge
