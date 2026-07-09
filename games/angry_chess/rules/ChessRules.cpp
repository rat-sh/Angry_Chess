#include "games/angry_chess/rules/ChessRules.hpp"
#include "games/angry_chess/ChessPieceTypes.hpp"
#include "games/angry_chess/pieces/Pawn.hpp"
#include "games/angry_chess/pieces/Knight.hpp"
#include "games/angry_chess/pieces/Bishop.hpp"
#include "games/angry_chess/pieces/Rook.hpp"
#include "games/angry_chess/pieces/Queen.hpp"
#include "games/angry_chess/pieces/King.hpp"
#include "games/angry_chess/rules/CheckDetector.hpp"
#include "games/angry_chess/rules/CastlingRule.hpp"
#include "games/angry_chess/rules/ChessWinCondition.hpp"
#include "engine/rules/MoveResult.hpp"

namespace angry_chess {
using namespace bge;

// ─── Pseudo-legal generation ──────────────────────────────────────────────────

std::vector<Move> ChessRules::pseudoLegal(const BoardState& state,
                                           Position from) const {
    std::vector<Move> moves;
    switch (state.typeAt(from)) {
        case PAWN:   moves = pawnMoves  (state, from); break;
        case KNIGHT: moves = knightMoves(state, from); break;
        case BISHOP: moves = bishopMoves(state, from); break;
        case ROOK:   moves = rookMoves  (state, from); break;
        case QUEEN:  moves = queenMoves (state, from); break;
        case KING:
            moves = kingMoves(state, from);
            addCastlingMoves(state, state.colorAt(from), moves);
            break;
        default: break;
    }
    return moves;
}

// ─── Filter for king-safety ───────────────────────────────────────────────────

std::vector<Move> ChessRules::filterLegal(const BoardState& state,
                                           Color             color,
                                           std::vector<Move> pseudos) const {
    std::vector<Move> legal;
    legal.reserve(pseudos.size());
    for (const auto& m : pseudos) {
        BoardState next = state.applyMove(m);
        if (!isInCheck(next, color)) legal.push_back(m);
    }
    return legal;
}

// ─── IGameRules impl ─────────────────────────────────────────────────────────

std::vector<Move> ChessRules::generateLegalMoves(const BoardState& state,
                                                  Position from) const {
    if (state.isEmpty(from)) return {};
    Color color = state.colorAt(from);
    return filterLegal(state, color, pseudoLegal(state, from));
}

std::vector<Move> ChessRules::generateAllLegalMoves(const BoardState& state,
                                                      Color color) const {
    std::vector<Move> all;
    all.reserve(40);
    for (int8_t r = 0; r < 8; ++r) {
        for (int8_t f = 0; f < 8; ++f) {
            Position pos{r, f};
            if (state.colorAt(pos) != color) continue;
            auto pm = pseudoLegal(state, pos);
            for (const auto& m : pm) {
                BoardState next = state.applyMove(m);
                if (!isInCheck(next, color)) all.push_back(m);
            }
        }
    }
    return all;
}

MoveResult ChessRules::validateMove(const BoardState& state,
                                     const Move& move) const {
    if (!move.from.isValid() || !move.to.isValid())
        return MoveResult::OutOfBounds;
    if (state.isEmpty(move.from))
        return MoveResult::NoPieceAtSource;

    Color moverColor = state.colorAt(move.from);
    if (moverColor != state.activePlayer)
        return MoveResult::WrongTurn;

    if (!state.isEmpty(move.to) && state.colorAt(move.to) == moverColor)
        return MoveResult::FriendlyFire;

    // Check if the destination matches any legal move
    auto legal = generateLegalMoves(state, move.from);
    for (const auto& lm : legal) {
        if (lm.from == move.from && lm.to == move.to) {
            // Handle promotion type validation
            if (hasFlag(lm.flags, MoveFlag::Promotion)) {
                PieceTypeID pt = move.promotionType;
                if (pt == QUEEN || pt == ROOK || pt == BISHOP || pt == KNIGHT ||
                    pt == 'q'  || pt == 'r'  || pt == 'b'   || pt == 'n') {
                    return MoveResult::Legal;
                }
                // Default to queen if not specified
                return MoveResult::Legal;
            }
            return MoveResult::Legal;
        }
    }

    // The move matches piece movement but leaves king in check, or is flat-out illegal
    auto pseudo = pseudoLegal(state, move.from);
    for (const auto& pm : pseudo) {
        if (pm.from == move.from && pm.to == move.to) {
            return MoveResult::LeavesKingInCheck;
        }
    }
    return MoveResult::Illegal;
}

GameStatus ChessRules::checkGameStatus(const BoardState& state,
                                        Color activePlayer) const {
    return evaluateGameStatus(state, activePlayer);
}

BoardState ChessRules::applyMove(const BoardState& state, const Move& move) const {
    // Resolve promotion type: char codes → piece type IDs
    Move resolved = move;
    if (hasFlag(move.flags, MoveFlag::Promotion)) {
        switch (move.promotionType) {
            case 'q': case 'Q': resolved.promotionType = QUEEN;  break;
            case 'r': case 'R': resolved.promotionType = ROOK;   break;
            case 'b': case 'B': resolved.promotionType = BISHOP; break;
            case 'n': case 'N': resolved.promotionType = KNIGHT; break;
            default:
                if (resolved.promotionType < PAWN || resolved.promotionType > KING)
                    resolved.promotionType = QUEEN; // default queen
                break;
        }
    }

    // Find the fully annotated move (for en-passant captureAt, castling secondaryMove)
    auto legal = generateLegalMoves(state, move.from);
    for (const auto& lm : legal) {
        if (lm.from == move.from && lm.to == move.to) {
            Move toApply = lm;
            if (hasFlag(move.flags, MoveFlag::Promotion))
                toApply.promotionType = resolved.promotionType;
            return state.applyMove(toApply);
        }
    }
    return state.applyMove(resolved); // fallback (should not happen after validateMove)
}

// ─── Initial position ─────────────────────────────────────────────────────────

BoardState ChessRules::initialState() const {
    BoardState s;
    s.activePlayer   = Color::White;
    s.castling       = CastlingRights{};
    s.halfMoveClock  = 0;
    s.fullMoveNumber = 1;

    auto setW = [&](int8_t r, int8_t f, PieceTypeID t) {
        s.setCell({r, f}, t, Color::White);
    };
    auto setB = [&](int8_t r, int8_t f, PieceTypeID t) {
        s.setCell({r, f}, t, Color::Black);
    };

    // White pieces (rank 0 = row 1)
    setW(0,0,ROOK);  setW(0,1,KNIGHT); setW(0,2,BISHOP);
    setW(0,3,QUEEN); setW(0,4,KING);
    setW(0,5,BISHOP);setW(0,6,KNIGHT); setW(0,7,ROOK);
    for (int8_t f = 0; f < 8; ++f) setW(1, f, PAWN);

    // Black pieces (rank 7 = row 8)
    setB(7,0,ROOK);  setB(7,1,KNIGHT); setB(7,2,BISHOP);
    setB(7,3,QUEEN); setB(7,4,KING);
    setB(7,5,BISHOP);setB(7,6,KNIGHT); setB(7,7,ROOK);
    for (int8_t f = 0; f < 8; ++f) setB(6, f, PAWN);

    return s;
}

} // namespace angry_chess
