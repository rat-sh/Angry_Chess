#pragma once
#include "engine/core/Types.hpp"
#include <string>
#include <string_view>

namespace bge {

/// A display theme: maps piece-type IDs and colors to visual symbols and labels.
/// The renderer calls ITheme to get symbols — it never knows what game is being played.
class ITheme {
public:
    virtual ~ITheme() = default;

    /// Unicode (or ASCII) symbol for a piece.
    /// `typeId` is the PieceTypeID registered by the plugin.
    [[nodiscard]] virtual std::string pieceSymbol(
        PieceTypeID typeId, Color color) const = 0;

    /// ANSI escape code for a light square background (e.g. "\033[48;5;223m").
    [[nodiscard]] virtual std::string lightSquare() const = 0;

    /// ANSI escape code for a dark square background.
    [[nodiscard]] virtual std::string darkSquare() const = 0;

    /// ANSI reset code.
    [[nodiscard]] virtual std::string reset() const = 0;

    /// Color code for white pieces.
    [[nodiscard]] virtual std::string whitePieceColor() const = 0;

    /// Color code for black pieces.
    [[nodiscard]] virtual std::string blackPieceColor() const = 0;

    /// Highlight color for selected square.
    [[nodiscard]] virtual std::string selectedSquare() const = 0;

    /// Highlight color for legal move target squares.
    [[nodiscard]] virtual std::string legalMoveSquare() const = 0;

    /// Highlight color for the last-move squares.
    [[nodiscard]] virtual std::string lastMoveSquare() const = 0;

    /// Highlight color when the king is in check.
    [[nodiscard]] virtual std::string checkSquare() const = 0;

    /// Short display name of the theme.
    [[nodiscard]] virtual std::string_view name() const = 0;

    /// Optional taunt/flavour text shown after events.
    [[nodiscard]] virtual std::string tauntOnCapture()  const { return ""; }
    [[nodiscard]] virtual std::string tauntOnCheck()    const { return ""; }
    [[nodiscard]] virtual std::string tauntOnCheckmate()const { return ""; }
};

} // namespace bge
