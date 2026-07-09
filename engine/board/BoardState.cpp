#include "engine/board/BoardState.hpp"

namespace bge {

Position BoardState::findFirst(PieceTypeID type, Color color) const noexcept {
    for (int8_t r = 0; r < 8; ++r) {
        for (int8_t f = 0; f < 8; ++f) {
            Position p{r, f};
            if (typeAt(p) == type && colorAt(p) == color)
                return p;
        }
    }
    return {}; // invalid
}

BoardState BoardState::applyMove(const Move& m) const noexcept {
    BoardState next = *this;

    // ── 1. Remove captured piece (handles en-passant where capture != to) ────
    if (m.captureAt.has_value()) {
        next.clearCell(*m.captureAt);
    }

    // ── 2. Move the primary piece ─────────────────────────────────────────────
    PieceTypeID moverType  = typeAt(m.from);
    Color       moverColor = colorAt(m.from);
    next.clearCell(m.from);

    // If promotion, change piece type
    PieceTypeID destType = hasFlag(m.flags, MoveFlag::Promotion)
                         ? m.promotionType
                         : moverType;
    next.setCell(m.to, destType, moverColor);

    // ── 3. Secondary move (castling: rook) ────────────────────────────────────
    if (m.secondaryMove.has_value()) {
        auto [secFrom, secTo] = *m.secondaryMove;
        PieceTypeID secType  = typeAt(secFrom);
        Color       secColor = colorAt(secFrom);
        next.clearCell(secFrom);
        next.setCell(secTo, secType, secColor);
    }

    // ── 4. Update en-passant target ────────────────────────────────────────────
    next.enPassantTarget = std::nullopt;
    if (hasFlag(m.flags, MoveFlag::DoublePush)) {
        // EP target is the square the pawn skipped over
        int8_t epRank = static_cast<int8_t>((m.from.rank + m.to.rank) / 2);
        next.enPassantTarget = Position{epRank, m.from.file};
    }

    // ── 5. Update castling rights ─────────────────────────────────────────────
    // If king moved → revoke both rights for that color
    // (chess plugin sets moverType; engine has no knowledge of piece semantics,
    //  but we track by position: if piece moves from e1/e8 it's the king)
    // Actually we rely on the plugin to set flags correctly; we do positional check:
    // Revoke kingside if rook moved from h-file corner, queenside from a-file corner.
    auto revokeByPosition = [&](Position from) {
        if (from.rank == 0) { // white
            if (from.file == 4) next.castling.revokeAll(Color::White);
            if (from.file == 7) next.castling.revokeKingside(Color::White);
            if (from.file == 0) next.castling.revokeQueenside(Color::White);
        } else if (from.rank == 7) { // black
            if (from.file == 4) next.castling.revokeAll(Color::Black);
            if (from.file == 7) next.castling.revokeKingside(Color::Black);
            if (from.file == 0) next.castling.revokeQueenside(Color::Black);
        }
        // Also revoke if captured piece was a rook on its home square
    };
    revokeByPosition(m.from);

    // ── 6. Half-move clock ────────────────────────────────────────────────────
    if (hasFlag(m.flags, MoveFlag::Capture) || hasFlag(m.flags, MoveFlag::EnPassant)) {
        next.halfMoveClock = 0;
    } else {
        ++next.halfMoveClock;
    }

    // ── 7. Full move number ───────────────────────────────────────────────────
    if (activePlayer == Color::Black) ++next.fullMoveNumber;

    // ── 8. Flip active player ─────────────────────────────────────────────────
    next.activePlayer = opposite(activePlayer);

    return next;
}

} // namespace bge
