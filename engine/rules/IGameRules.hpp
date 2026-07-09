#pragma once
#include "engine/core/Types.hpp"
#include "engine/board/BoardState.hpp"
#include "engine/rules/Move.hpp"
#include "engine/rules/MoveResult.hpp"
#include <vector>

namespace bge {

/// The central rules interface. Every game plugin implements this.
/// The engine knows nothing about chess — it only calls these methods.
class IGameRules {
public:
    virtual ~IGameRules() = default;

    // ── Validation ────────────────────────────────────────────────────────────

    /// Check whether a move is fully legal (including king-safety check).
    [[nodiscard]] virtual MoveResult validateMove(
        const BoardState& state,
        const Move& move) const = 0;

    // ── Move Generation ───────────────────────────────────────────────────────

    /// Generate all fully-legal moves for the piece at `from`.
    /// Returns empty vector if the square is empty or the piece has no legal moves.
    [[nodiscard]] virtual std::vector<Move> generateLegalMoves(
        const BoardState& state,
        Position from) const = 0;

    /// Generate all legal moves for `color` across the entire board.
    [[nodiscard]] virtual std::vector<Move> generateAllLegalMoves(
        const BoardState& state,
        Color color) const = 0;

    // ── Status ────────────────────────────────────────────────────────────────

    /// Compute the current game status for the player who is about to move.
    [[nodiscard]] virtual GameStatus checkGameStatus(
        const BoardState& state,
        Color activePlayer) const = 0;

    // ── Move Application ─────────────────────────────────────────────────────

    /// Apply a validated move and return the resulting state.
    /// Delegates to BoardState::applyMove for mechanical changes, then
    /// applies any game-specific side effects (e.g. Zobrist hash update).
    [[nodiscard]] virtual BoardState applyMove(
        const BoardState& state,
        const Move& move) const = 0;

    // ── Board Setup ───────────────────────────────────────────────────────────

    /// Return the initial board state for a new game.
    [[nodiscard]] virtual BoardState initialState() const = 0;

    /// Human-readable name for this ruleset.
    [[nodiscard]] virtual std::string_view name() const = 0;
};

} // namespace bge
