#include "games/angry_chess/pieces/Bishop.hpp"
#include "games/angry_chess/ChessPieceTypes.hpp"

namespace angry_chess {
using namespace bge;

void addDiagonalMoves(const BoardState& state, Position from,
                      Color color, std::vector<Move>& out) {
    static constexpr int8_t dirs[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
    for (auto& [dr, df] : dirs) {
        for (int step = 1; step < 8; ++step) {
            int8_t r = static_cast<int8_t>(from.rank + dr * step);
            int8_t f = static_cast<int8_t>(from.file + df * step);
            if (!inBounds(r, f)) break;
            Position dest{r, f};
            Color dc = state.colorAt(dest);
            if (dc == color) break;
            if (dc == Color::None) out.push_back(normalMove(from, dest));
            else { out.push_back(captureMove(from, dest)); break; }
        }
    }
}

std::vector<Move> bishopMoves(const BoardState& state, Position from) {
    std::vector<Move> moves;
    Color color = state.colorAt(from);
    if (color == Color::None) return moves;
    addDiagonalMoves(state, from, color, moves);
    return moves;
}
} // namespace angry_chess
