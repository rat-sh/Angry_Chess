#pragma once
#include "engine/theme/ITheme.hpp"
#include "games/angry_chess/ChessPieceTypes.hpp"
#include <string>

namespace angry_chess {

/// Classic FIDE theme: Unicode chess symbols on cream/brown ANSI squares.
class ClassicTheme final : public bge::ITheme {
public:
    [[nodiscard]] std::string pieceSymbol(bge::PieceTypeID typeId,
                                          bge::Color color) const override {
        if (color == bge::Color::White) {
            switch (typeId) {
                case KING:   return "♔";
                case QUEEN:  return "♕";
                case ROOK:   return "♖";
                case BISHOP: return "♗";
                case KNIGHT: return "♘";
                case PAWN:   return "♙";
                default:     return "?";
            }
        } else {
            switch (typeId) {
                case KING:   return "♚";
                case QUEEN:  return "♛";
                case ROOK:   return "♜";
                case BISHOP: return "♝";
                case KNIGHT: return "♞";
                case PAWN:   return "♟";
                default:     return "?";
            }
        }
    }

    // ANSI 256-colour palette — cream light, warm brown dark
    [[nodiscard]] std::string lightSquare()      const override { return "\033[48;5;230m"; }
    [[nodiscard]] std::string darkSquare()       const override { return "\033[48;5;94m";  }
    [[nodiscard]] std::string reset()            const override { return "\033[0m";         }
    [[nodiscard]] std::string whitePieceColor()  const override { return "\033[1;38;5;255m"; }
    [[nodiscard]] std::string blackPieceColor()  const override { return "\033[1;38;5;232m"; }
    [[nodiscard]] std::string selectedSquare()   const override { return "\033[48;5;226m"; }
    [[nodiscard]] std::string legalMoveSquare()  const override { return "\033[48;5;77m";  }
    [[nodiscard]] std::string lastMoveSquare()   const override { return "\033[48;5;178m"; }
    [[nodiscard]] std::string checkSquare()      const override { return "\033[48;5;196m"; }
    [[nodiscard]] std::string_view name()        const override { return "classic";         }

    // No taunts for the classic theme
    [[nodiscard]] std::string tauntOnCapture()   const override { return ""; }
    [[nodiscard]] std::string tauntOnCheck()     const override { return "Check!"; }
    [[nodiscard]] std::string tauntOnCheckmate() const override { return "Checkmate!"; }
};

} // namespace angry_chess
