#include "engine/renderer/ConsoleRenderer.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace bge {

// ─── ANSI reset ───────────────────────────────────────────────────────────────
static constexpr const char* kReset   = "\033[0m";
static constexpr const char* kBold    = "\033[1m";
static constexpr const char* kClear   = "\033[2J\033[H";

// ── Helpers ───────────────────────────────────────────────────────────────────
static HighlightType highlightAt(Position pos,
                                 const std::vector<SquareHighlight>& hl) {
    for (auto& h : hl)
        if (h.pos == pos) return h.type;
    return HighlightType::None;
}

// ─── ConsoleRenderer ─────────────────────────────────────────────────────────

void ConsoleRenderer::clearScreen() const {
    std::cout << kClear;
}

void ConsoleRenderer::beginFrame() {
    clearScreen();
}

std::string ConsoleRenderer::squareBg(Position pos,
                                       HighlightType hl,
                                       const ITheme& theme) const {
    switch (hl) {
        case HighlightType::Selected:   return theme.selectedSquare();
        case HighlightType::LegalMove:  return theme.legalMoveSquare();
        case HighlightType::LastMove:   return theme.lastMoveSquare();
        case HighlightType::Check:      return theme.checkSquare();
        case HighlightType::Capture:    return theme.checkSquare();
        default: break;
    }
    bool light = (pos.rank + pos.file) % 2 == 0;
    return light ? theme.lightSquare() : theme.darkSquare();
}

void ConsoleRenderer::drawBoard(const RenderContext& ctx) {
    if (!ctx.state || !ctx.theme) return;

    const BoardState& bs    = *ctx.state;
    const ITheme&     theme = *ctx.theme;

    // Determine rendering direction
    bool flipped = ctx.flipped;
    auto displayRank = [&](int8_t logicalRank) -> int8_t {
        return flipped ? logicalRank : static_cast<int8_t>(7 - logicalRank);
    };
    auto displayFile = [&](int8_t logicalFile) -> int8_t {
        return flipped ? static_cast<int8_t>(7 - logicalFile) : logicalFile;
    };

    std::cout << "\n";

    // ── Column labels ─────────────────────────────────────────────────────────
    std::cout << "     ";
    for (int8_t f = 0; f < 8; ++f) {
        int8_t df = flipped ? static_cast<int8_t>(7 - f) : f;
        std::cout << "  " << static_cast<char>('a' + df) << "  ";
    }
    std::cout << "\n";

    // ── Top border ────────────────────────────────────────────────────────────
    std::cout << "   " << kBold << " ┌";
    for (int f = 0; f < 8; ++f) std::cout << (f ? "┬" : "") << "────";
    std::cout << "┐" << kReset << "\n";

    // ── Board rows ────────────────────────────────────────────────────────────
    for (int8_t row = 0; row < 8; ++row) {
        int8_t rank = displayRank(row); // logical rank from top to bottom display

        // Row separator (not for first row)
        if (row > 0) {
            std::cout << "   " << kBold << " ├";
            for (int f = 0; f < 8; ++f) std::cout << (f ? "┼" : "") << "────";
            std::cout << "┤" << kReset << "\n";
        }

        // Rank label
        std::cout << " " << kBold << (rank + 1) << kReset << " " << kBold << " │" << kReset;

        for (int8_t col = 0; col < 8; ++col) {
            int8_t file = displayFile(col);
            Position pos{rank, file};

            HighlightType hl = highlightAt(pos, ctx.highlights);
            std::string bg   = squareBg(pos, hl, theme);

            PieceTypeID type  = bs.typeAt(pos);
            Color       color = bs.colorAt(pos);

            std::string symbol = " ";
            std::string pieceColor;
            if (type != kNoPiece) {
                symbol     = theme.pieceSymbol(type, color);
                pieceColor = (color == Color::White)
                           ? theme.whitePieceColor()
                           : theme.blackPieceColor();
            }

            // Each cell: " X " where X is the piece symbol (unicode = 1 char wide)
            std::cout << bg << pieceColor
                      << " " << symbol << " "
                      << kReset << kBold << "│" << kReset;
        }

        // Rank label (right side)
        std::cout << " " << kBold << (rank + 1) << kReset << "\n";
    }

    // ── Bottom border ─────────────────────────────────────────────────────────
    std::cout << "   " << kBold << " └";
    for (int f = 0; f < 8; ++f) std::cout << (f ? "┴" : "") << "────";
    std::cout << "┘" << kReset << "\n";

    // ── Column labels (bottom) ────────────────────────────────────────────────
    std::cout << "     ";
    for (int8_t f = 0; f < 8; ++f) {
        int8_t df = flipped ? static_cast<int8_t>(7 - f) : f;
        std::cout << "  " << static_cast<char>('a' + df) << "  ";
    }
    std::cout << "\n\n";
}

void ConsoleRenderer::drawStatus(const RenderContext& ctx) {
    // Separator line
    std::cout << "\033[38;5;240m"
              << "  ────────────────────────────────────────────\n"
              << kReset;

    if (!ctx.statusLine.empty()) {
        std::cout << "  " << kBold << ctx.statusLine << kReset << "\n";
    }
    if (!ctx.messageLine.empty()) {
        // Messages use a distinct colour (amber)
        std::cout << "  \033[38;5;214m" << ctx.messageLine << kReset << "\n";
    }
    std::cout << "\n";
}

void ConsoleRenderer::drawPrompt(std::string_view prompt) {
    std::cout << "  \033[38;5;117m" << prompt << kReset;
    std::cout.flush();
}

void ConsoleRenderer::endFrame() {
    std::cout.flush();
}

void ConsoleRenderer::onResize() {
    // Terminal resize: nothing to do for this renderer
}

} // namespace bge
