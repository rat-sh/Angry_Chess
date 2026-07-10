#pragma once
#include "engine/renderer/IRenderer.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <array>
#include <string>

namespace bge {

// ─── Theme palette (matches fun_chess exactly) ────────────────────────────────
struct SDL2Palette {
    SDL_Color lightBg;        // light square background
    SDL_Color darkBg;         // dark square background
    SDL_Color lightTrace;     // last-move highlight on light square
    SDL_Color darkTrace;      // last-move highlight on dark square
    SDL_Color lightMoves;     // legal-move dot color on light square
    SDL_Color darkMoves;      // legal-move dot color on dark square
    SDL_Color selectedLight;  // selected piece on light square
    SDL_Color selectedDark;   // selected piece on dark square
    const char* name;
};

/// Full graphical renderer for the Angry Chess engine.
/// Implements IRenderer using SDL2 + SDL2_ttf.
/// Board size: 800×800  Square size: 100×100
/// Status bar: 60px below the board  Total window: 800×860
class SDL2Renderer final : public IRenderer {
public:
    static constexpr int kSqSize  = 100;
    static constexpr int kRows    = 8;
    static constexpr int kCols    = 8;
    static constexpr int kBoardW  = kSqSize * kCols;   // 800
    static constexpr int kBoardH  = kSqSize * kRows;   // 800
    static constexpr int kStatusH = 60;
    static constexpr int kWinW    = kBoardW;
    static constexpr int kWinH    = kBoardH + kStatusH;

    explicit SDL2Renderer();
    ~SDL2Renderer() override;

    // IRenderer interface
    void beginFrame() override;
    void drawBoard(const RenderContext& ctx) override;
    void drawStatus(const RenderContext& ctx) override;
    void drawPrompt(std::string_view prompt) override;
    void endFrame() override;
    void onResize() override {}

    // Theme cycling (T key)
    void nextTheme();
    int  themeIndex() const { return themeIdx_; }

    // Promotion picker — blocks until user clicks one of 4 pieces
    // Returns: QUEEN=5 ROOK=4 BISHOP=3 KNIGHT=2 (ChessPieceTypes IDs)
    PieceTypeID showPromotionPicker(Color color) override;

    bool isOpen() const { return window_ != nullptr; }

    // Expose SDL handles for external menu/overlay rendering
    SDL_Window*   window()   const { return window_;   }
    SDL_Renderer* renderer() const { return renderer_; }
    TTF_Font*     fontMed()  const { return fontMed_;  }
    TTF_Font*     fontBig()  const { return fontBig_;  }

private:
    SDL_Window*   window_{nullptr};
    SDL_Renderer* renderer_{nullptr};
    TTF_Font*     fontLarge_{nullptr};  // 64px — piece symbols
    TTF_Font*     fontMed_{nullptr};    // 20px — coordinates, status
    TTF_Font*     fontBig_{nullptr};    // 48px — game-over text

    int themeIdx_{0};

    static const std::array<SDL2Palette, 4> kPalettes;

    const SDL2Palette& palette() const { return kPalettes[themeIdx_]; }

    // Internal draw helpers
    void drawSquareBg(int rank, int file, const RenderContext& ctx);
    void drawPieceSymbol(int rank, int file, PieceTypeID type, Color color, const RenderContext& ctx);
    void drawLegalDot(int rank, int file, const RenderContext& ctx);
    void drawLastMoveTrace(const RenderContext& ctx);
    void drawHighlights(const RenderContext& ctx);
    void drawCoordinates(const RenderContext& ctx);
    void drawGameOver(const RenderContext& ctx);
    void drawStatusBar(const RenderContext& ctx);

    // Coordinate helpers
    SDL_Rect squareRect(int rank, int file, const RenderContext& ctx) const;
    std::pair<int,int> rankFileFromPixel(int x, int y, bool flipped) const;

    // Render text centred in a rect
    void renderText(const std::string& text, TTF_Font* font,
                    SDL_Color color, int cx, int cy);
    void renderTextLeft(const std::string& text, TTF_Font* font,
                        SDL_Color color, int x, int y);

    // Filled circle (for legal move dots)
    void fillCircle(int cx, int cy, int r, SDL_Color c);

    // Semi-transparent filled rect
    void fillRectAlpha(SDL_Rect rect, SDL_Color color, Uint8 alpha);
};

} // namespace bge
