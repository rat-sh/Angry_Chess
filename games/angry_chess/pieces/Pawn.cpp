#include "games/angry_chess/pieces/Pawn.hpp"
#include "games/angry_chess/ChessPieceTypes.hpp"

namespace angry_chess {

using namespace bge;


std::vector<Move> pawnMoves(const BoardState& state, Position from) {
    std::vector<Move> moves;
    Color color = state.colorAt(from);
    if (color == Color::None) return moves;

    int dir        = (color == Color::White) ?  1 : -1;
    int startRank  = (color == Color::White) ?  1 :  6;
    int promoRank  = (color == Color::White) ?  7 :  0;

    int8_t r = from.rank;
    int8_t f = from.file;

    // ── Single push ──────────────────────────────────────────────────────────
    int8_t r1 = static_cast<int8_t>(r + dir);
    if (inBounds(r1, f) && state.isEmpty({r1, f})) {
        if (r1 == promoRank) {
            // Generate all four promotion moves
            for (PieceTypeID pt : {QUEEN, ROOK, BISHOP, KNIGHT}) {
                Move m = normalMove(from, {r1, f});
                m.flags |= MoveFlag::Promotion;
                m.promotionType = pt;
                moves.push_back(m);
            }
        } else {
            moves.push_back(normalMove(from, {r1, f}));
        }

        // ── Double push (only from starting rank, only if path clear) ────────
        if (r == startRank) {
            int8_t r2 = static_cast<int8_t>(r + 2 * dir);
            if (inBounds(r2, f) && state.isEmpty({r2, f})) {
                Move m = normalMove(from, {r2, f});
                m.flags |= MoveFlag::DoublePush;
                moves.push_back(m);
            }
        }
    }

    // ── Diagonal captures ────────────────────────────────────────────────────
    for (int8_t df : {-1, 1}) {
        int8_t cf = static_cast<int8_t>(f + df);
        if (!inBounds(r1, cf)) continue;
        Position dest{r1, cf};

        // Normal capture
        if (!state.isEmpty(dest) && state.colorAt(dest) != color) {
            if (r1 == promoRank) {
                for (PieceTypeID pt : {QUEEN, ROOK, BISHOP, KNIGHT}) {
                    Move m   = captureMove(from, dest);
                    m.flags |= MoveFlag::Promotion;
                    m.promotionType = pt;
                    moves.push_back(m);
                }
            } else {
                moves.push_back(captureMove(from, dest));
            }
        }

        // En-passant capture
        if (state.enPassantTarget.has_value() &&
            dest == *state.enPassantTarget) {
            Move m;
            m.from  = from;
            m.to    = dest;
            m.flags = MoveFlag::EnPassant | MoveFlag::Capture;
            // The captured pawn is on the same rank as 'from', same file as 'dest'
            m.captureAt = Position{from.rank, dest.file};
            moves.push_back(m);
        }
    }

    return moves;
}

} // namespace angry_chess
