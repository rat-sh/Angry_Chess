#include "chess/rules/CheckDetector.hpp"
#include "chess/ChessPieceTypes.hpp"
#include "chess/pieces/Pawn/Pawn.hpp"
#include "chess/pieces/Knight/Knight.hpp"
#include "chess/pieces/Bishop/Bishop.hpp"
#include "chess/pieces/Rook/Rook.hpp"
#include "chess/pieces/Queen/Queen.hpp"
#include "chess/pieces/King/King.hpp"

namespace angry_chess {
using namespace bge;

// Generate pseudo-legal moves for any piece at `from` (used for attack detection)
static std::vector<Move> pseudoLegal(const BoardState& state, Position from) {
    PieceTypeID type = state.typeAt(from);
    switch (type) {
        case PAWN:   return pawnMoves  (state, from);
        case KNIGHT: return knightMoves(state, from);
        case BISHOP: return bishopMoves(state, from);
        case ROOK:   return rookMoves  (state, from);
        case QUEEN:  return queenMoves (state, from);
        case KING:   return kingMoves  (state, from);
        default:     return {};
    }
}

bool isSquareAttacked(const BoardState& state,
                      Position          square,
                      Color             attackerColor) {
    for (int8_t r = 0; r < 8; ++r) {
        for (int8_t f = 0; f < 8; ++f) {
            Position pos{r, f};
            if (state.colorAt(pos) != attackerColor) continue;
            auto moves = pseudoLegal(state, pos);
            for (const auto& m : moves) {
                if (m.to == square) return true;
            }
        }
    }
    return false;
}

bool isInCheck(const BoardState& state, Color color) {
    Position kingPos = state.findFirst(KING, color);
    if (!kingPos.isValid()) return false; // no king (shouldn't happen)
    return isSquareAttacked(state, kingPos, opposite(color));
}

} // namespace angry_chess
