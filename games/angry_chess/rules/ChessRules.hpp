#pragma once
#include "engine/rules/IGameRules.hpp"

namespace angry_chess {

/// Full FIDE-legal chess rule implementation.
/// Implements IGameRules — the engine only talks to this interface.
class ChessRules final : public bge::IGameRules {
public:
    ChessRules() = default;

    [[nodiscard]] bge::MoveResult validateMove(
        const bge::BoardState& state,
        const bge::Move& move) const override;

    [[nodiscard]] std::vector<bge::Move> generateLegalMoves(
        const bge::BoardState& state,
        bge::Position from) const override;

    [[nodiscard]] std::vector<bge::Move> generateAllLegalMoves(
        const bge::BoardState& state,
        bge::Color color) const override;

    [[nodiscard]] bge::GameStatus checkGameStatus(
        const bge::BoardState& state,
        bge::Color activePlayer) const override;

    [[nodiscard]] bge::BoardState applyMove(
        const bge::BoardState& state,
        const bge::Move& move) const override;

    [[nodiscard]] bge::BoardState initialState() const override;

    [[nodiscard]] std::string_view name() const override { return "Angry Chess (FIDE)"; }

private:
    /// Generate all pseudo-legal moves for the piece at `from`.
    [[nodiscard]] std::vector<bge::Move> pseudoLegal(
        const bge::BoardState& state,
        bge::Position from) const;

    /// Filter pseudo-legal moves by king-safety.
    [[nodiscard]] std::vector<bge::Move> filterLegal(
        const bge::BoardState& state,
        bge::Color color,
        std::vector<bge::Move> pseudos) const;
};

} // namespace angry_chess
