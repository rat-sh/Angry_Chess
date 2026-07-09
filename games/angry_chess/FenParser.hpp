#pragma once
#include "engine/board/BoardState.hpp"
#include <string>
#include <stdexcept>

namespace bge {

/// Minimal FEN (Forsyth-Edwards Notation) parser for chess starting positions.
/// Produces a BoardState from a FEN string — useful for loading puzzles,
/// custom games, and AI testing scenarios.
///
/// FEN format: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
///   [placement] [activeColor] [castling] [en-passant] [halfmove] [fullmove]
struct FenParser {
    /// Parse a FEN string and produce a BoardState with chess piece type IDs.
    /// Throws std::invalid_argument on malformed input.
    static BoardState parse(std::string_view fen);

    /// Generate FEN from a BoardState (requires chess piece semantics).
    static std::string generate(const BoardState& state);
};

} // namespace bge
