#include "games/angry_chess/pieces/Queen.hpp"
#include "games/angry_chess/pieces/Bishop.hpp"
#include "games/angry_chess/pieces/Rook.hpp"
#include "games/angry_chess/ChessPieceTypes.hpp"

namespace angry_chess {
using namespace bge;

std::vector<Move> queenMoves(const BoardState& state, Position from) {
    std::vector<Move> moves;
    Color color = state.colorAt(from);
    if (color == Color::None) return moves;
    addOrthogonalMoves(state, from, color, moves); // rook-like
    addDiagonalMoves  (state, from, color, moves); // bishop-like
    return moves;
}
} // namespace angry_chess
