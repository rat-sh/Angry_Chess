#pragma once
#include "engine/core/Types.hpp"
#include "engine/board/BoardState.hpp"
#include "engine/rules/Move.hpp"
#include <chrono>

namespace bge {

/// Interface for AI move selectors.
/// Implement this to plug in any AI strategy
/// (random, minimax, MCTS, engine UCI, neural net, …).
class IAIPlayer {
public:
    virtual ~IAIPlayer() = default;

    /// Select a move for `forPlayer` in the given `state`.
    /// `rules` provides legal-move generation and state application.
    /// `timeLimit` is a soft budget hint; implementations SHOULD respect it.
    ///
    /// Returns Move{} (invalid move) if no legal move exists (should not happen
    /// in a well-formed game — the engine checks for terminal state first).
    [[nodiscard]] virtual Move selectMove(
        const BoardState&   state,
        const class IGameRules& rules,
        Color               forPlayer,
        std::chrono::milliseconds timeLimit) = 0;

    /// Optional: reset search state between games.
    virtual void reset() {}

    /// Human-readable name for this AI player.
    [[nodiscard]] virtual std::string_view name() const = 0;
};

} // namespace bge
