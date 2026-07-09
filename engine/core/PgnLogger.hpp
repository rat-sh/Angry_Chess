#pragma once
#include "engine/board/BoardState.hpp"
#include "engine/rules/Move.hpp"
#include <string>
#include <vector>
#include <sstream>

namespace bge {

/// Portable Game Notation (PGN) logger.
/// Records moves as the game progresses and can export a valid PGN string.
///
/// Usage:
///   PgnLogger pgn("White", "Black");
///   pgn.recordMove(state, move);
///   std::cout << pgn.toPgn();
class PgnLogger {
public:
    explicit PgnLogger(std::string whiteName = "Human",
                       std::string blackName = "Engine",
                       std::string eventName = "Angry Chess")
        : whiteName_(std::move(whiteName))
        , blackName_(std::move(blackName))
        , eventName_(std::move(eventName))
    {}

    /// Record a move in SAN-like format (simplified algebraic notation).
    void recordMove(const BoardState& stateBefore, const Move& move) {
        std::string san;
        // Piece prefix (P is omitted for pawns)
        PieceTypeID type = stateBefore.typeAt(move.from);
        if (type != kNoPiece) {
            const char pieceChars[] = {' ', ' ', 'N', 'B', 'R', 'Q', 'K'};
            if (type < sizeof(pieceChars) && pieceChars[type] != ' ')
                san += pieceChars[type];
        }
        san += move.from.toAlgebraic();
        // Capture separator
        if (!stateBefore.isEmpty(move.to))
            san += 'x';
        san += move.to.toAlgebraic();
        // Promotion suffix
        if (move.promotionType != kNoPiece) {
            const char promoChars[] = {' ', ' ', 'N', 'B', 'R', 'Q', 'K'};
            san += '=';
            if (move.promotionType < sizeof(promoChars))
                san += promoChars[move.promotionType];
        }
        moves_.push_back(san);
    }

    void setResult(std::string_view result) { result_ = result; }

    [[nodiscard]] std::string toPgn() const {
        std::ostringstream ss;
        // Headers
        ss << "[Event \"" << eventName_ << "\"]\n"
           << "[White \"" << whiteName_ << "\"]\n"
           << "[Black \"" << blackName_ << "\"]\n"
           << "[Result \"" << result_   << "\"]\n\n";

        // Move list
        for (std::size_t i = 0; i < moves_.size(); ++i) {
            if (i % 2 == 0) ss << (i / 2 + 1) << ". ";
            ss << moves_[i] << " ";
            if (i % 2 == 1) ss << "\n"; // newline after each full move pair
        }
        ss << result_ << "\n";
        return ss.str();
    }

    void clear() { moves_.clear(); result_ = "*"; }

    [[nodiscard]] std::size_t moveCount() const { return moves_.size(); }

private:
    std::string             whiteName_;
    std::string             blackName_;
    std::string             eventName_;
    std::string             result_{"*"};
    std::vector<std::string> moves_;
};

} // namespace bge
