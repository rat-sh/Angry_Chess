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

// ─── Castling Rights ─────────────────────────────────────────────────────────
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
enum class GameStatus : uint8_t {
    Ongoing,
    Check,
    Checkmate,
    Stalemate,
    Draw,
    Resigned,
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
