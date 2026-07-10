#include "chess/rules/win/ChessWinCondition.hpp"
#include "chess/rules/CheckDetector.hpp"
#include "chess/ChessPieceTypes.hpp"
#include "chess/pieces/Pawn/Pawn.hpp"
#include "chess/pieces/Knight/Knight.hpp"
#include "chess/pieces/Bishop/Bishop.hpp"
#include "chess/pieces/Rook/Rook.hpp"
#include "chess/pieces/Queen/Queen.hpp"
#include "chess/pieces/King/King.hpp"
#include "chess/rules/movement/CastlingRule.hpp"

namespace angry_chess {
using namespace bge;

/// Generates pseudo-legal moves for a piece (used here to test legal-move availability)
static std::vector<Move> pseudoLegalForPiece(const BoardState& state, Position from) {
    switch (state.typeAt(from)) {
        case PAWN:   return pawnMoves  (state, from);
        case KNIGHT: return knightMoves(state, from);
        case BISHOP: return bishopMoves(state, from);
        case ROOK:   return rookMoves  (state, from);
        case QUEEN:  return queenMoves (state, from);
        case KING:   { auto m = kingMoves(state, from);
                       addCastlingMoves(state, state.colorAt(from), m);
                       return m; }
        default:     return {};
    }
}

/// Returns true if `color` has at least one legal move.
static bool hasAnyLegalMove(const BoardState& state, Color color) {
    for (int8_t r = 0; r < 8; ++r) {
        for (int8_t f = 0; f < 8; ++f) {
            Position pos{r, f};
            if (state.colorAt(pos) != color) continue;
            for (const auto& m : pseudoLegalForPiece(state, pos)) {
                // Test: apply move, check if own king is still safe
                BoardState next = state.applyMove(m);
                if (!isInCheck(next, color)) return true;
            }
        }
    }
    return false;
}

GameStatus evaluateGameStatus(const BoardState& state, Color activePlayer) {
    bool inCheck  = isInCheck(state, activePlayer);
    bool canMove  = hasAnyLegalMove(state, activePlayer);

    if (!canMove) {
        return inCheck ? GameStatus::Checkmate : GameStatus::Stalemate;
    }
    // Fifty-move rule draw
    if (state.halfMoveClock >= 100) return GameStatus::Draw;

    return inCheck ? GameStatus::Check : GameStatus::Ongoing;
}

} // namespace angry_chess
