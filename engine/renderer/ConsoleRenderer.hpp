#pragma once
#include "engine/renderer/IRenderer.hpp"

namespace bge {

/// Full-featured ANSI + Unicode terminal renderer.
/// Renders an 8×8 board with box-drawing characters, Unicode chess symbols,
/// ANSI 256-colour backgrounds, and highlighted squares.
class ConsoleRenderer final : public IRenderer {
public:
    ConsoleRenderer() = default;

    void beginFrame() override;
    void drawBoard(const RenderContext& ctx) override;
    void drawStatus(const RenderContext& ctx) override;
    void drawPrompt(std::string_view prompt) override;
    void endFrame() override;
    void onResize() override;

private:
    /// Returns the ANSI background for a given square and highlight.
    std::string squareBg(Position pos,
                         HighlightType hl,
                         const ITheme& theme) const;

    /// Clears the terminal (ANSI escape).
    void clearScreen() const;
};

} // namespace bge
