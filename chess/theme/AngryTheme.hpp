#pragma once
#include "engine/theme/ITheme.hpp"
#include "chess/ChessPieceTypes.hpp"
#include <string>

namespace angry_chess {

/// 😡 Angry theme: rage emoji symbols, fiery board colours, and snarky taunts.
class AngryTheme final : public bge::ITheme {
public:
    [[nodiscard]] std::string pieceSymbol(bge::PieceTypeID typeId,
                                          bge::Color color) const override {
        // White pieces get hot fire symbols; black pieces get skull/dark symbols
        if (color == bge::Color::White) {
            switch (typeId) {
                case KING:   return "♔";
                case QUEEN:  return "♕";
                case ROOK:   return "♖";
                case BISHOP: return "♗";
                case KNIGHT: return "♘";
                case PAWN:   return "♙";
                default:     return "*";
            }
        } else {
            switch (typeId) {
                case KING:   return "♚";
                case QUEEN:  return "♛";
                case ROOK:   return "♜";
                case BISHOP: return "♝";
                case KNIGHT: return "♞";
                case PAWN:   return "♟";
                default:     return "*";
            }
        }
    }

    // Fiery orange-red tones for the board
    [[nodiscard]] std::string lightSquare()      const override { return "\033[48;5;208m"; }
    [[nodiscard]] std::string darkSquare()       const override { return "\033[48;5;52m";  }
    [[nodiscard]] std::string reset()            const override { return "\033[0m";         }
    [[nodiscard]] std::string whitePieceColor()  const override { return "\033[1;38;5;226m"; }
    [[nodiscard]] std::string blackPieceColor()  const override { return "\033[1;38;5;196m"; }
    [[nodiscard]] std::string selectedSquare()   const override { return "\033[48;5;220m"; }
    [[nodiscard]] std::string legalMoveSquare()  const override { return "\033[48;5;28m";  }
    [[nodiscard]] std::string lastMoveSquare()   const override { return "\033[48;5;214m"; }
    [[nodiscard]] std::string checkSquare()      const override { return "\033[48;5;160m"; }
    [[nodiscard]] std::string_view name()        const override { return "angry";           }

    [[nodiscard]] std::string tauntOnCapture() const override {
        return "💥 DESTROYED! That piece is GONE!";
    }
    [[nodiscard]] std::string tauntOnCheck() const override {
        return "😡 CHECK! Your king is SHAKING!";
    }
    [[nodiscard]] std::string tauntOnCheckmate() const override {
        return "🔥 CHECKMATE! TOTAL ANNIHILATION! 🔥";
    }
};

} // namespace angry_chess
