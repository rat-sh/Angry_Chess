#pragma once
#include <cstdint>

namespace bge {

/// Per-move validation result.
enum class MoveResult : uint8_t {
    Legal,             ///< Move is fully legal; may be applied.
    Illegal,           ///< Move violates piece movement rules.
    OutOfBounds,       ///< Source or destination outside the board.
    WrongTurn,         ///< Moving a piece that doesn't belong to the active player.
    LeavesKingInCheck, ///< Move is pseudo-legal but exposes own king.
    NoPieceAtSource,   ///< Source square is empty.
    FriendlyFire,      ///< Destination occupied by a friendly piece.
};

inline bool isLegal(MoveResult r) noexcept {
    return r == MoveResult::Legal;
}

inline const char* toString(MoveResult r) noexcept {
    switch (r) {
        case MoveResult::Legal:             return "Legal";
        case MoveResult::Illegal:           return "Illegal move";
        case MoveResult::OutOfBounds:       return "Position out of bounds";
        case MoveResult::WrongTurn:         return "Not your piece";
        case MoveResult::LeavesKingInCheck: return "Move leaves king in check";
        case MoveResult::NoPieceAtSource:   return "No piece at source square";
        case MoveResult::FriendlyFire:      return "Cannot capture your own piece";
        default:                            return "Unknown";
    }
}

} // namespace bge
