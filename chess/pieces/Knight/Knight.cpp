#include "chess/pieces/Knight/Knight.hpp"
#include "chess/ChessPieceTypes.hpp"

namespace angry_chess {
using namespace bge;

std::vector<Move> knightMoves(const BoardState& state, Position from) {
    std::vector<Move> moves;
    Color color = state.colorAt(from);
    if (color == Color::None) return moves;

    static constexpr int8_t offsets[8][2] = {
        {2,1},{2,-1},{-2,1},{-2,-1},{1,2},{1,-2},{-1,2},{-1,-2}
    };
    for (auto& [dr, df] : offsets) {
        int8_t r = static_cast<int8_t>(from.rank + dr);
        int8_t f = static_cast<int8_t>(from.file + df);
        if (!inBounds(r, f)) continue;
        Position dest{r, f};
        Color dc = state.colorAt(dest);
        if (dc == color) continue; // friendly piece
        if (dc == Color::None) moves.push_back(normalMove(from, dest));
        else                   moves.push_back(captureMove(from, dest));
    }
    return moves;
}
} // namespace angry_chess
