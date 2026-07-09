#include "games/angry_chess/ai/ChessEvaluator.hpp"
#include "games/angry_chess/ChessPieceTypes.hpp"

namespace angry_chess {
using namespace bge;

// ─── Material values (centipawns) ────────────────────────────────────────────
static constexpr int MATERIAL[7] = {
    0,      // kNoPiece (0)
    100,    // PAWN
    320,    // KNIGHT
    330,    // BISHOP
    500,    // ROOK
    900,    // QUEEN
    20000,  // KING
};

// ─── Piece-square tables (from White's perspective, rank 0 = white back rank) ─
// Values in centipawns; add to piece value when on that square.

static constexpr int PST_PAWN[8][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {10, 10, 20, 30, 30, 20, 10, 10},
    { 5,  5, 10, 25, 25, 10,  5,  5},
    { 0,  0,  0, 20, 20,  0,  0,  0},
    { 5, -5,-10,  0,  0,-10, -5,  5},
    { 5, 10, 10,-20,-20, 10, 10,  5},
    { 0,  0,  0,  0,  0,  0,  0,  0},
};

static constexpr int PST_KNIGHT[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50},
    {-40,-20,  0,  0,  0,  0,-20,-40},
    {-30,  0, 10, 15, 15, 10,  0,-30},
    {-30,  5, 15, 20, 20, 15,  5,-30},
    {-30,  0, 15, 20, 20, 15,  0,-30},
    {-30,  5, 10, 15, 15, 10,  5,-30},
    {-40,-20,  0,  5,  5,  0,-20,-40},
    {-50,-40,-30,-30,-30,-30,-40,-50},
};

static constexpr int PST_BISHOP[8][8] = {
    {-20,-10,-10,-10,-10,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5, 10, 10,  5,  0,-10},
    {-10,  5,  5, 10, 10,  5,  5,-10},
    {-10,  0, 10, 10, 10, 10,  0,-10},
    {-10, 10, 10, 10, 10, 10, 10,-10},
    {-10,  5,  0,  0,  0,  0,  5,-10},
    {-20,-10,-10,-10,-10,-10,-10,-20},
};

static constexpr int PST_ROOK[8][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 5, 10, 10, 10, 10, 10, 10,  5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    { 0,  0,  0,  5,  5,  0,  0,  0},
};

static constexpr int PST_QUEEN[8][8] = {
    {-20,-10,-10, -5, -5,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5,  5,  5,  5,  0,-10},
    { -5,  0,  5,  5,  5,  5,  0, -5},
    {  0,  0,  5,  5,  5,  5,  0, -5},
    {-10,  5,  5,  5,  5,  5,  0,-10},
    {-10,  0,  5,  0,  0,  0,  0,-10},
    {-20,-10,-10, -5, -5,-10,-10,-20},
};

static constexpr int PST_KING_MIDGAME[8][8] = {
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-20,-30,-30,-40,-40,-30,-30,-20},
    {-10,-20,-20,-20,-20,-20,-20,-10},
    { 20, 20,  0,  0,  0,  0, 20, 20},
    { 20, 30, 10,  0,  0, 10, 30, 20},
};

static int pstValue(PieceTypeID type, int8_t rank, int8_t file, Color color) {
    // Mirror rank for black (they advance in the opposite direction)
    int8_t r = (color == Color::White) ? rank : static_cast<int8_t>(7 - rank);
    switch (type) {
        case PAWN:   return PST_PAWN  [r][file];
        case KNIGHT: return PST_KNIGHT[r][file];
        case BISHOP: return PST_BISHOP[r][file];
        case ROOK:   return PST_ROOK  [r][file];
        case QUEEN:  return PST_QUEEN [r][file];
        case KING:   return PST_KING_MIDGAME[r][file];
        default:     return 0;
    }
}

int chessEvaluate(const BoardState& state) {
    int score = 0;
    for (int8_t r = 0; r < 8; ++r) {
        for (int8_t f = 0; f < 8; ++f) {
            Position pos{r, f};
            PieceTypeID type = state.typeAt(pos);
            if (type == kNoPiece) continue;
            Color color = state.colorAt(pos);
            int pieceScore = MATERIAL[type] + pstValue(type, r, f, color);
            if (color == Color::White) score += pieceScore;
            else                       score -= pieceScore;
        }
    }
    return score;
}

} // namespace angry_chess
