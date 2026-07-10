#pragma once
#include <cstdint>
#include <cmath>
#include <string>
#include <string_view>

namespace bge {

// ─── Piece Identity ──────────────────────────────────────────────────────────
// PieceTypeID = 0 means "empty square".
// Values 1..127  → piece type registered by a plugin.
// On the board grid:  positive value = White, negative = Black.
using PieceTypeID = uint8_t;
constexpr PieceTypeID kNoPiece = 0;

// ─── Player Color ─────────────────────────────────────────────────────────────
enum class Color : int8_t {
    White = 1,
    Black = -1,
    None  = 0
};

inline Color opposite(Color c) noexcept {
    return (c == Color::White) ? Color::Black
         : (c == Color::Black) ? Color::White
         : Color::None;
}

inline std::string_view colorName(Color c) noexcept {
    if (c == Color::White) return "White";
    if (c == Color::Black) return "Black";
    return "None";
}

// ─── Board Position ───────────────────────────────────────────────────────────
// rank: 0 = rank-1 (white's back rank), 7 = rank-8
// file: 0 = a-file, 7 = h-file
struct Position {
    int8_t rank{-1};
    int8_t file{-1};

    [[nodiscard]] bool isValid() const noexcept {
        return rank >= 0 && rank < 8 && file >= 0 && file < 8;
    }

    bool operator==(const Position& o) const noexcept {
        return rank == o.rank && file == o.file;
    }
    bool operator!=(const Position& o) const noexcept { return !(*this == o); }
    bool operator<(const Position& o)  const noexcept {
        return rank < o.rank || (rank == o.rank && file < o.file);
    }

    // Algebraic notation, e.g. "e4"
    [[nodiscard]] std::string toAlgebraic() const {
        if (!isValid()) return "--";
        std::string s;
        s += static_cast<char>('a' + file);
        s += static_cast<char>('1' + rank);
        return s;
    }

    static Position fromAlgebraic(std::string_view sq) {
        if (sq.size() < 2) return {};
        int8_t f = static_cast<int8_t>(sq[0] - 'a');
        int8_t r = static_cast<int8_t>(sq[1] - '1');
        if (f < 0 || f > 7 || r < 0 || r > 7) return {};
        return {r, f};
    }
};

// ─── Secondary Move Rights ────────────────────────────────────────────────────
/// Generic bitmask for tracking game-specific special move availability.
/// Each game plugin defines what the bits mean (e.g., castling rights in chess).
/// Use 0 for "no special rights available".
struct SecondaryMoveRights {
    uint8_t flags{0xFF}; ///< All bits set = all rights available (game plugin interprets)

    void revoke(uint8_t mask) noexcept { flags &= ~mask; }
    [[nodiscard]] bool has(uint8_t mask) const noexcept { return (flags & mask) != 0; }
    void grant(uint8_t mask) noexcept { flags |= mask; }
};

// ─── Legacy Castling Rights Shim ─────────────────────────────────────────────
// Provided for game plugin compatibility only.
// New code should use SecondaryMoveRights.
struct CastlingRights {
    bool whiteKingside  : 1 {true};
    bool whiteQueenside : 1 {true};
    bool blackKingside  : 1 {true};
    bool blackQueenside : 1 {true};

    bool kingsideFor(Color c) const noexcept {
        return c == Color::White ? whiteKingside : blackKingside;
    }
    bool queensideFor(Color c) const noexcept {
        return c == Color::White ? whiteQueenside : blackQueenside;
    }
    void revokeAll(Color c) noexcept {
        if (c == Color::White) { whiteKingside = whiteQueenside = false; }
        else                   { blackKingside = blackQueenside = false; }
    }
    void revokeKingside(Color c)  noexcept {
        if (c == Color::White) whiteKingside  = false; else blackKingside  = false;
    }
    void revokeQueenside(Color c) noexcept {
        if (c == Color::White) whiteQueenside = false; else blackQueenside = false;
    }
};

// ─── Game Status ─────────────────────────────────────────────────────────────
/// Generic game status codes returned by IGameRules::checkGameStatus().
/// Game plugins return these to the engine; the engine has zero knowledge of
/// what they mean in game terms — only Ongoing vs. non-Ongoing is significant
/// for loop control. Individual values carry semantic meaning for rendering.
enum class GameStatus : uint8_t {
    Ongoing,      ///< Game is in progress; current player can move.
    Check,        ///< Game-specific: current player is under threat (e.g. chess check).
    Checkmate,    ///< Game-specific: current player lost (no escape).
    Stalemate,    ///< No legal moves but not under direct threat; typically a draw.
    Draw,         ///< Draw by rule (fifty-move, repetition, etc.).
    Resigned,     ///< Active player resigned.
};

// ─── Highlight type (used by renderer) ───────────────────────────────────────
enum class HighlightType : uint8_t {
    None,
    Selected,
    LegalMove,
    LastMove,
    Check,
    Capture,
};

// ─── Player mode ──────────────────────────────────────────────────────────────
enum class PlayerMode : uint8_t {
    Human,
    AI,
};

} // namespace bge
