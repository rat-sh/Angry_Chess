#include "chess/pieces/King/King.hpp"
#include "chess/ChessPieceTypes.hpp"

namespace angry_chess {
using namespace bge;

std::vector<Move> kingMoves(const BoardState& state, Position from) {
    std::vector<Move> moves;
    Color color = state.colorAt(from);
    if (color == Color::None) return moves;

    for (int8_t dr = -1; dr <= 1; ++dr) {
        for (int8_t df = -1; df <= 1; ++df) {
            if (dr == 0 && df == 0) continue;
            int8_t r = static_cast<int8_t>(from.rank + dr);
            int8_t f = static_cast<int8_t>(from.file + df);
            if (!inBounds(r, f)) continue;
            Position dest{r, f};
            Color dc = state.colorAt(dest);
            if (dc == color) continue;
            if (dc == Color::None) moves.push_back(normalMove(from, dest));
            else                   moves.push_back(captureMove(from, dest));
        }
    }
    return moves;
}
} // namespace angry_chess
