#include "chess/notation/Fen/FenParser.hpp"
#include "chess/ChessPieceTypes.hpp"
#include <sstream>
#include <stdexcept>
#include <charconv>

namespace angry_chess {

using bge::BoardState;
using bge::Color;
using bge::Position;
using bge::PieceTypeID;
using bge::CastlingRights;
using bge::kNoPiece;

static PieceTypeID fenCharToPiece(char c) {
    switch (std::tolower(c)) {
        case 'p': return PAWN;
        case 'n': return KNIGHT;
        case 'b': return BISHOP;
        case 'r': return ROOK;
        case 'q': return QUEEN;
        case 'k': return KING;
        default:  return kNoPiece;
    }
}

static char pieceToFenChar(PieceTypeID id, Color color) {
    char c = '?';
    switch (id) {
        case PAWN:   c = 'p'; break;
        case KNIGHT: c = 'n'; break;
        case BISHOP: c = 'b'; break;
        case ROOK:   c = 'r'; break;
        case QUEEN:  c = 'q'; break;
        case KING:   c = 'k'; break;
        default:     return '?';
    }
    return (color == Color::White) ? static_cast<char>(std::toupper(c)) : c;
}

BoardState FenParser::parse(std::string_view fen) {
    BoardState s;
    s.castling = CastlingRights{false, false, false, false};

    // Split into tokens
    std::string fenStr{fen};
    std::istringstream ss(fenStr);
    std::string placement, activeColor, castling, enPassant, halfmove, fullmove;
    ss >> placement >> activeColor >> castling >> enPassant >> halfmove >> fullmove;

    if (placement.empty())
        throw std::invalid_argument("FEN: missing piece placement");

    // ── 1. Piece placement ────────────────────────────────────────────────────
    // FEN rank 8 → our rank 7, FEN rank 1 → our rank 0
    int8_t rank = 7;
    int8_t file  = 0;
    for (char c : placement) {
        if (c == '/') {
            --rank;
            file = 0;
        } else if (std::isdigit(c)) {
            file += static_cast<int8_t>(c - '0');
        } else {
            Color color = std::isupper(c) ? Color::White : Color::Black;
            PieceTypeID type = fenCharToPiece(c);
            if (type != kNoPiece)
                s.setCell({rank, file}, type, color);
            ++file;
        }
    }

    // ── 2. Active color ────────────────────────────────────────────────────────
    s.activePlayer = (activeColor == "b") ? Color::Black : Color::White;

    // ── 3. Castling rights ─────────────────────────────────────────────────────
    if (castling != "-") {
        for (char c : castling) {
            switch (c) {
                case 'K': s.castling.whiteKingside  = true; break;
                case 'Q': s.castling.whiteQueenside = true; break;
                case 'k': s.castling.blackKingside  = true; break;
                case 'q': s.castling.blackQueenside = true; break;
                default: break;
            }
        }
    }

    // ── 4. En-passant target ───────────────────────────────────────────────────
    if (enPassant != "-" && enPassant.size() == 2) {
        s.enPassantTarget = Position::fromAlgebraic(enPassant);
    }

    // ── 5. Clocks ─────────────────────────────────────────────────────────────
    if (!halfmove.empty()) {
        try { s.halfMoveClock = static_cast<uint16_t>(std::stoi(halfmove)); }
        catch (...) {}
    }
    if (!fullmove.empty()) {
        try { s.fullMoveNumber = static_cast<uint32_t>(std::stoi(fullmove)); }
        catch (...) {}
    }

    return s;
}

std::string FenParser::generate(const BoardState& state) {
    std::string fen;

    // ── 1. Piece placement (rank 7 → rank 0) ─────────────────────────────────
    for (int8_t r = 7; r >= 0; --r) {
        int emptyCount = 0;
        for (int8_t f = 0; f < 8; ++f) {
            Position pos{r, f};
            if (state.isEmpty(pos)) {
                ++emptyCount;
            } else {
                if (emptyCount > 0) {
                    fen += static_cast<char>('0' + emptyCount);
                    emptyCount = 0;
                }
                fen += pieceToFenChar(state.typeAt(pos), state.colorAt(pos));
            }
        }
        if (emptyCount > 0) fen += static_cast<char>('0' + emptyCount);
        if (r > 0) fen += '/';
    }

    // ── 2. Active color ────────────────────────────────────────────────────────
    fen += ' ';
    fen += (state.activePlayer == Color::Black) ? 'b' : 'w';

    // ── 3. Castling rights ─────────────────────────────────────────────────────
    fen += ' ';
    std::string castStr;
    if (state.castling.whiteKingside)  castStr += 'K';
    if (state.castling.whiteQueenside) castStr += 'Q';
    if (state.castling.blackKingside)  castStr += 'k';
    if (state.castling.blackQueenside) castStr += 'q';
    fen += castStr.empty() ? "-" : castStr;

    // ── 4. En-passant ─────────────────────────────────────────────────────────
    fen += ' ';
    if (state.enPassantTarget.has_value())
        fen += state.enPassantTarget->toAlgebraic();
    else
        fen += '-';

    // ── 5. Clocks ─────────────────────────────────────────────────────────────
    fen += ' ';
    fen += std::to_string(state.halfMoveClock);
    fen += ' ';
    fen += std::to_string(state.fullMoveNumber);

    return fen;
}

} // namespace angry_chess
