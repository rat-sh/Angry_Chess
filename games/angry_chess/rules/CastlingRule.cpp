#include "games/angry_chess/rules/CastlingRule.hpp"
#include "games/angry_chess/rules/CheckDetector.hpp"
#include "games/angry_chess/ChessPieceTypes.hpp"

namespace angry_chess {
using namespace bge;

void addCastlingMoves(const BoardState& state,
                      Color             color,
                      std::vector<Move>& out) {
    // King may not castle while in check
    if (isInCheck(state, color)) return;

    int8_t rank = (color == Color::White) ? 0 : 7;
    Color  opp  = opposite(color);

    // ── Kingside (O-O) ───────────────────────────────────────────────────────
    if (state.castling.kingsideFor(color)) {
        Position f1{rank, 5}, g1{rank, 6}, h1{rank, 7};
        if (state.isEmpty(f1) && state.isEmpty(g1) &&
            state.typeAt(h1) == ROOK && state.colorAt(h1) == color &&
            !isSquareAttacked(state, f1, opp) &&
            !isSquareAttacked(state, g1, opp)) {

            Position kingFrom{rank, 4}, kingTo{rank, 6};
            Move m;
            m.from          = kingFrom;
            m.to            = kingTo;
            m.flags         = MoveFlag::Castle;
            m.secondaryMove = {{h1, f1}}; // rook h1→f1
            out.push_back(m);
        }
    }

    // ── Queenside (O-O-O) ─────────────────────────────────────────────────────
    if (state.castling.queensideFor(color)) {
        Position b1{rank, 1}, c1{rank, 2}, d1{rank, 3}, a1{rank, 0};
        if (state.isEmpty(b1) && state.isEmpty(c1) && state.isEmpty(d1) &&
            state.typeAt(a1) == ROOK && state.colorAt(a1) == color &&
            !isSquareAttacked(state, c1, opp) &&
            !isSquareAttacked(state, d1, opp)) {

            Position kingFrom{rank, 4}, kingTo{rank, 2};
            Move m;
            m.from          = kingFrom;
            m.to            = kingTo;
            m.flags         = MoveFlag::Castle;
            m.secondaryMove = {{a1, d1}}; // rook a1→d1
            out.push_back(m);
        }
    }
}

} // namespace angry_chess
