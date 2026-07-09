#pragma once
#include "engine/theme/ITheme.hpp"
#include <string>

namespace bge {

/// Fallback theme for debugging or unregistered piece types.
/// Shows piece type-ID as a number; uses plain dark/light terminal colours.
class DefaultTheme final : public ITheme {
public:
    [[nodiscard]] std::string pieceSymbol(PieceTypeID typeId, Color) const override {
        if (typeId == kNoPiece) return " ";
        return std::to_string(typeId);
    }

    // ANSI 256-colour: light square = colour 230 (cream), dark = colour 94 (brown)
    [[nodiscard]] std::string lightSquare()      const override { return "\033[48;5;230m"; }
    [[nodiscard]] std::string darkSquare()       const override { return "\033[48;5;94m";  }
    [[nodiscard]] std::string reset()            const override { return "\033[0m";         }
    [[nodiscard]] std::string whitePieceColor()  const override { return "\033[38;5;255m"; }
    [[nodiscard]] std::string blackPieceColor()  const override { return "\033[38;5;232m"; }
    [[nodiscard]] std::string selectedSquare()   const override { return "\033[48;5;226m"; }
    [[nodiscard]] std::string legalMoveSquare()  const override { return "\033[48;5;34m";  }
    [[nodiscard]] std::string lastMoveSquare()   const override { return "\033[48;5;178m"; }
    [[nodiscard]] std::string checkSquare()      const override { return "\033[48;5;196m"; }
    [[nodiscard]] std::string_view name()        const override { return "default";         }
};

} // namespace bge
