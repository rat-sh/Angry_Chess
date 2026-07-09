#pragma once
#include "engine/core/Types.hpp"
#include "engine/rules/Move.hpp"
#include <array>
#include <optional>

namespace bge {

// ─── Piece Cell ───────────────────────────────────────────────────────────────
/// Compact cell encoding used by BoardState::grid.
/// grid[rank][file] == 0  → empty
/// grid[rank][file] >  0  → White piece of type grid[rank][file]
/// grid[rank][file] <  0  → Black piece of type -grid[rank][file]
using CellData = int8_t;

// ─── BoardState ───────────────────────────────────────────────────────────────
/// Pure value-semantic board snapshot.  Copyable in O(1) ~72 bytes.
/// Designed for fast cloning inside AI search trees.
/// Applying a move returns a NEW BoardState; the original is never mutated.
struct BoardState {
    // 8×8 grid, rank-major (rank 0 = white's back rank)
    std::array<std::array<CellData, 8>, 8> grid{};

    Color           activePlayer{Color::White};
    CastlingRights  castling{};
    std::optional<Position> enPassantTarget{}; ///< Square behind a double-pushed pawn
    uint16_t        halfMoveClock{0};          ///< Fifty-move rule counter
    uint32_t        fullMoveNumber{1};
    uint64_t        zobristHash{0};            ///< Incremental hash for TT

    // ── Cell accessors ───────────────────────────────────────────────────────
    [[nodiscard]] CellData rawAt(Position p) const noexcept {
        return grid[static_cast<std::size_t>(p.rank)][static_cast<std::size_t>(p.file)];
    }
    [[nodiscard]] bool isEmpty(Position p) const noexcept { return rawAt(p) == 0; }
    [[nodiscard]] PieceTypeID typeAt(Position p) const noexcept {
        auto v = rawAt(p);
        return static_cast<PieceTypeID>(v < 0 ? -v : v);
    }
    [[nodiscard]] Color colorAt(Position p) const noexcept {
        auto v = rawAt(p);
        if (v > 0) return Color::White;
        if (v < 0) return Color::Black;
        return Color::None;
    }

    void setCell(Position p, PieceTypeID type, Color color) noexcept {
        grid[static_cast<std::size_t>(p.rank)][static_cast<std::size_t>(p.file)] =
            static_cast<CellData>(
                type == kNoPiece ? 0 :
                color == Color::White ? static_cast<int8_t>(type)
                                     : -static_cast<int8_t>(type));
    }
    void clearCell(Position p) noexcept {
        grid[static_cast<std::size_t>(p.rank)][static_cast<std::size_t>(p.file)] = 0;
    }

    // ── Derived queries ──────────────────────────────────────────────────────
    /// Find the first position matching (type, color).  Returns invalid Position if not found.
    [[nodiscard]] Position findFirst(PieceTypeID type, Color color) const noexcept;

    // ── Move application ─────────────────────────────────────────────────────
    /// Returns a new BoardState after applying the move.
    /// Handles: normal move, capture, en-passant removal, castling rook,
    ///          pawn promotion, en-passant target update, halfmove clock.
    /// Does NOT validate legality — call IGameRules::validateMove first.
    [[nodiscard]] BoardState applyMove(const Move& m) const noexcept;

    // ── Operators ────────────────────────────────────────────────────────────
    bool operator==(const BoardState& o) const noexcept {
        return grid == o.grid &&
               activePlayer == o.activePlayer &&
               castling.whiteKingside  == o.castling.whiteKingside &&
               castling.whiteQueenside == o.castling.whiteQueenside &&
               castling.blackKingside  == o.castling.blackKingside &&
               castling.blackQueenside == o.castling.blackQueenside &&
               enPassantTarget == o.enPassantTarget;
    }
    bool operator!=(const BoardState& o) const noexcept { return !(*this == o); }
};

} // namespace bge
