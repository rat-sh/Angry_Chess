#pragma once
#include "engine/core/Types.hpp"
#include <cstdint>
#include <optional>
#include <utility>

namespace bge {

// ─── Move Flags ───────────────────────────────────────────────────────────────
enum class MoveFlag : uint8_t {
    Normal    = 0,
    Capture   = 1 << 0,   // Destination square has an enemy piece
    Castle    = 1 << 1,   // King-side or queen-side castling
    EnPassant = 1 << 2,   // En-passant pawn capture
    Promotion = 1 << 3,   // Pawn promotion
    DoublePush= 1 << 4,   // Pawn's initial two-square push (sets EP target)
};

inline MoveFlag operator|(MoveFlag a, MoveFlag b) noexcept {
    return static_cast<MoveFlag>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}
inline MoveFlag operator&(MoveFlag a, MoveFlag b) noexcept {
    return static_cast<MoveFlag>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}
inline MoveFlag& operator|=(MoveFlag& a, MoveFlag b) noexcept { return a = a | b; }
inline bool hasFlag(MoveFlag flags, MoveFlag test) noexcept {
    return (static_cast<uint8_t>(flags) & static_cast<uint8_t>(test)) != 0;
}

// ─── Move ─────────────────────────────────────────────────────────────────────
/// A fully-specified, engine-agnostic move description.
/// The chess plugin fills in the special fields when generating moves so that
/// BoardState::applyMove() needs zero game-specific knowledge.
struct Move {
    Position from{};
    Position to{};
    MoveFlag flags{MoveFlag::Normal};

    /// For pawn promotion: the PieceTypeID to promote to (0 = no promotion).
    PieceTypeID promotionType{kNoPiece};

    /// Piece that will be captured.
    /// • Normal captures   : equals 'to'
    /// • En-passant capture: differs from 'to' (the pawn behind the destination)
    std::optional<Position> captureAt{};

    /// Optional secondary piece movement (used for castling: rook from → to).
    std::optional<std::pair<Position, Position>> secondaryMove{};

    [[nodiscard]] bool isValid() const noexcept {
        return from.isValid() && to.isValid() && from != to;
    }

    bool operator==(const Move& o) const noexcept {
        return from == o.from && to == o.to &&
               flags == o.flags && promotionType == o.promotionType;
    }
    bool operator!=(const Move& o) const noexcept { return !(*this == o); }
};

// Convenience factories
inline Move normalMove(Position from, Position to) noexcept {
    Move m; m.from = from; m.to = to; m.flags = MoveFlag::Normal; return m;
}
inline Move captureMove(Position from, Position to) noexcept {
    Move m; m.from = from; m.to = to;
    m.flags = MoveFlag::Capture;
    m.captureAt = to;
    return m;
}

} // namespace bge
