#include "engine/renderer/SDL2Renderer.hpp"
#include "games/angry_chess/ChessPieceTypes.hpp"
#include <stdexcept>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <array>

namespace bge {

// ─── Palette table (exact fun_chess colours) ──────────────────────────────────
const std::array<SDL2Palette, 4> SDL2Renderer::kPalettes = {{
    // Green
    { {234,235,200,255}, {119,154, 88,255},
      {244,247,116,255}, {172,195, 51,255},
      {200,200, 80,200}, {140,170, 40,200},
      {244,247,116,255}, {172,195, 51,255},
      "Green" },
    // Brown
    { {235,209,166,255}, {165,117, 80,255},
      {245,234,100,255}, {209,185, 59,255},
      {210,180, 60,200}, {170,140, 30,200},
      {245,234,100,255}, {209,185, 59,255},
      "Brown" },
    // Blue
    { {229,228,200,255}, { 60, 95,135,255},
      {123,187,227,255}, { 43,119,191,255},
      { 80,160,210,200}, { 30,100,170,200},
      {123,187,227,255}, { 43,119,191,255},
      "Blue" },
    // Gray
    { {120,119,118,255}, { 86, 85, 84,255},
      { 99,126,143,255}, { 82,102,128,255},
      { 90,120,140,200}, { 70,100,120,200},
      { 99,126,143,255}, { 82,102,128,255},
      "Gray" },
}};

// ─── Unicode symbols for all pieces (classical, no emoji) ─────────────────────
static const char* pieceSymbol(PieceTypeID id, Color color) {
    using namespace angry_chess;
    if (color == Color::White) {
        switch (id) {
            case KING:   return "♔";
            case QUEEN:  return "♕";
            case ROOK:   return "♖";
            case BISHOP: return "♗";
            case KNIGHT: return "♘";
            case PAWN:   return "♙";
            default:     return "?";
        }
    } else {
        switch (id) {
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

// ─── Font search paths ─────────────────────────────────────────────────────────
static const char* kFontPaths[] = {
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    "/usr/share/fonts/truetype/freefont/FreeSerif.ttf",
    "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
    "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
    nullptr
};

static TTF_Font* loadFont(int pt) {
    for (int i = 0; kFontPaths[i]; ++i) {
        TTF_Font* f = TTF_OpenFont(kFontPaths[i], pt);
        if (f) return f;
    }
    return nullptr;
}

// ─── Constructor ──────────────────────────────────────────────────────────────
SDL2Renderer::SDL2Renderer() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        throw std::runtime_error(std::string("SDL_Init: ") + SDL_GetError());
    if (TTF_Init() != 0)
        throw std::runtime_error(std::string("TTF_Init: ") + TTF_GetError());

    window_ = SDL_CreateWindow(
        "Angry Chess",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        kWinW, kWinH,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (!window_)
        throw std::runtime_error(std::string("SDL_CreateWindow: ") + SDL_GetError());

    renderer_ = SDL_CreateRenderer(window_, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_)
        throw std::runtime_error(std::string("SDL_CreateRenderer: ") + SDL_GetError());

    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);

    fontLarge_ = loadFont(72);  // pieces
    fontMed_   = loadFont(18);  // coords, status
    fontBig_   = loadFont(52);  // game-over headline

    if (!fontLarge_ || !fontMed_ || !fontBig_)
        throw std::runtime_error("Could not load any TTF font. Install dejavu-fonts.");
}

SDL2Renderer::~SDL2Renderer() {
    if (fontLarge_) TTF_CloseFont(fontLarge_);
    if (fontMed_)   TTF_CloseFont(fontMed_);
    if (fontBig_)   TTF_CloseFont(fontBig_);
    if (renderer_)  SDL_DestroyRenderer(renderer_);
    if (window_)    SDL_DestroyWindow(window_);
    TTF_Quit();
    SDL_Quit();
}

// ─── Theme ────────────────────────────────────────────────────────────────────
void SDL2Renderer::nextTheme() {
    themeIdx_ = (themeIdx_ + 1) % static_cast<int>(kPalettes.size());
}

// ─── IRenderer interface ──────────────────────────────────────────────────────
void SDL2Renderer::beginFrame() {
    SDL_SetRenderDrawColor(renderer_, 30, 30, 30, 255);
    SDL_RenderClear(renderer_);
}

void SDL2Renderer::endFrame() {
    SDL_RenderPresent(renderer_);
}

void SDL2Renderer::drawPrompt(std::string_view /*prompt*/) {
    // Input handled by SDL2InputHandler events; nothing to draw here
}

// ─── Square geometry ──────────────────────────────────────────────────────────
SDL_Rect SDL2Renderer::squareRect(int rank, int file, const RenderContext& ctx) const {
    int col = ctx.flipped ? (7 - file) : file;
    int row = ctx.flipped ? rank       : (7 - rank);
    return { col * kSqSize, row * kSqSize, kSqSize, kSqSize };
}

// ─── Helpers ──────────────────────────────────────────────────────────────────
void SDL2Renderer::renderText(const std::string& text, TTF_Font* font,
                               SDL_Color color, int cx, int cy) {
    if (!font || text.empty()) return;
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer_, surf);
    SDL_Rect dst = { cx - surf->w / 2, cy - surf->h / 2, surf->w, surf->h };
    SDL_FreeSurface(surf);
    if (tex) { SDL_RenderCopy(renderer_, tex, nullptr, &dst); SDL_DestroyTexture(tex); }
}

void SDL2Renderer::renderTextLeft(const std::string& text, TTF_Font* font,
                                   SDL_Color color, int x, int y) {
    if (!font || text.empty()) return;
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer_, surf);
    SDL_Rect dst = { x, y, surf->w, surf->h };
    SDL_FreeSurface(surf);
    if (tex) { SDL_RenderCopy(renderer_, tex, nullptr, &dst); SDL_DestroyTexture(tex); }
}

void SDL2Renderer::fillCircle(int cx, int cy, int r, SDL_Color c) {
    SDL_SetRenderDrawColor(renderer_, c.r, c.g, c.b, c.a);
    for (int dy = -r; dy <= r; ++dy) {
        int dx = static_cast<int>(std::sqrt(r*r - dy*dy));
        SDL_RenderDrawLine(renderer_, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

void SDL2Renderer::fillRectAlpha(SDL_Rect rect, SDL_Color color, Uint8 alpha) {
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, alpha);
    SDL_RenderFillRect(renderer_, &rect);
}

// ─── Board drawing ────────────────────────────────────────────────────────────
void SDL2Renderer::drawBoard(const RenderContext& ctx) {
    if (!ctx.state || !ctx.theme) return;

    const auto& pal = palette();

    // ── 1. Square backgrounds + last-move trace ───────────────────────────────
    for (int8_t rank = 0; rank < 8; ++rank) {
        for (int8_t file = 0; file < 8; ++file) {
            bool light = (rank + file) % 2 == 0;
            SDL_Color bg = light ? pal.lightBg : pal.darkBg;

            // Last-move trace tint
            bool isTrace = false;
            if (ctx.lastMove.has_value()) {
                auto& lm = *ctx.lastMove;
                if ((lm.from.rank == rank && lm.from.file == file) ||
                    (lm.to.rank   == rank && lm.to.file   == file)) {
                    bg = light ? pal.lightTrace : pal.darkTrace;
                    isTrace = true;
                }
            }

            // Selected piece tint (overrides trace)
            for (auto& h : ctx.highlights) {
                if (h.pos.rank == rank && h.pos.file == file) {
                    if (h.type == HighlightType::Selected) {
                        bg = light ? pal.selectedLight : pal.selectedDark;
                    } else if (h.type == HighlightType::Check) {
                        bg = {200, 60, 60, 255};
                    }
                    break;
                }
            }

            SDL_Rect r = squareRect(rank, file, ctx);
            SDL_SetRenderDrawColor(renderer_, bg.r, bg.g, bg.b, bg.a);
            SDL_RenderFillRect(renderer_, &r);

            (void)isTrace;
        }
    }

    // ── 2. Coordinates ────────────────────────────────────────────────────────
    drawCoordinates(ctx);

    // ── 3. Legal-move dots ────────────────────────────────────────────────────
    for (auto& h : ctx.highlights) {
        if (h.type == HighlightType::LegalMove || h.type == HighlightType::Capture) {
            bool light = (h.pos.rank + h.pos.file) % 2 == 0;
            SDL_Color dotColor = light ? pal.lightMoves : pal.darkMoves;

            SDL_Rect sq = squareRect(h.pos.rank, h.pos.file, ctx);
            int cx = sq.x + kSqSize / 2;
            int cy = sq.y + kSqSize / 2;

            if (h.type == HighlightType::Capture) {
                // Ring on capture squares
                SDL_SetRenderDrawColor(renderer_, dotColor.r, dotColor.g, dotColor.b, 180);
                for (int t = 0; t < 6; ++t) {
                    SDL_Rect ring = { sq.x + t, sq.y + t, kSqSize - 2*t, kSqSize - 2*t };
                    SDL_RenderDrawRect(renderer_, &ring);
                }
            } else {
                // Solid dot
                fillCircle(cx, cy, kSqSize / 6, {dotColor.r, dotColor.g, dotColor.b, 180});
            }
        }
    }

    // ── 4. Piece symbols ──────────────────────────────────────────────────────
    for (int8_t rank = 0; rank < 8; ++rank) {
        for (int8_t file = 0; file < 8; ++file) {
            Position pos{rank, file};
            PieceTypeID type = ctx.state->typeAt(pos);
            if (type == kNoPiece) continue;
            Color c = ctx.state->colorAt(pos);

            SDL_Rect sq = squareRect(rank, file, ctx);
            int cx = sq.x + kSqSize / 2;
            int cy = sq.y + kSqSize / 2;

            // Shadow (slight offset)
            SDL_Color shadow = {0, 0, 0, 120};
            SDL_Surface* ssurf = TTF_RenderUTF8_Blended(fontLarge_, pieceSymbol(type, c), shadow);
            if (ssurf) {
                SDL_Texture* stex = SDL_CreateTextureFromSurface(renderer_, ssurf);
                SDL_Rect dst = { cx - ssurf->w/2 + 2, cy - ssurf->h/2 + 2, ssurf->w, ssurf->h };
                SDL_FreeSurface(ssurf);
                if (stex) { SDL_RenderCopy(renderer_, stex, nullptr, &dst); SDL_DestroyTexture(stex); }
            }

            // Piece (white = near-white, black = near-black for contrast)
            SDL_Color pieceCol = (c == Color::White)
                ? SDL_Color{255, 255, 255, 255}
                : SDL_Color{20,  20,  20,  255};
            renderText(pieceSymbol(type, c), fontLarge_, pieceCol, cx, cy);
        }
    }

    // ── 5. Hover outline ──────────────────────────────────────────────────────
    for (auto& h : ctx.highlights) {
        if (h.type == HighlightType::None) {
            SDL_Rect sq = squareRect(h.pos.rank, h.pos.file, ctx);
            SDL_SetRenderDrawColor(renderer_, 180, 180, 180, 180);
            for (int t = 0; t < 3; ++t) {
                SDL_Rect border = { sq.x + t, sq.y + t, kSqSize - 2*t, kSqSize - 2*t };
                SDL_RenderDrawRect(renderer_, &border);
            }
        }
    }

    // ── 6. Game-over overlay ──────────────────────────────────────────────────
    drawGameOver(ctx);
}

void SDL2Renderer::drawCoordinates(const RenderContext& ctx) {
    const auto& pal = palette();
    for (int i = 0; i < 8; ++i) {
        // Rank numbers (left edge)
        int8_t rank = ctx.flipped ? static_cast<int8_t>(i) : static_cast<int8_t>(7 - i);
        bool   light = (rank + 0) % 2 == 0;
        SDL_Color coordColor = light ? pal.darkBg : pal.lightBg;

        std::string rankLabel = std::to_string(rank + 1);
        renderTextLeft(rankLabel, fontMed_, coordColor,
                       5, i * kSqSize + 5);

        // File letters (bottom edge)
        int8_t file = ctx.flipped ? static_cast<int8_t>(7 - i) : static_cast<int8_t>(i);
        light = (0 + file) % 2 == 0;
        coordColor = light ? pal.darkBg : pal.lightBg;
        std::string fileLabel(1, static_cast<char>('a' + file));
        renderTextLeft(fileLabel, fontMed_, coordColor,
                       i * kSqSize + kSqSize - 16, kBoardH - 20);
    }
}

void SDL2Renderer::drawGameOver(const RenderContext& ctx) {
    // Only show if status is checkmate, stalemate, draw, or resigned
    bool over = !ctx.messageLine.empty() &&
                (ctx.messageLine.find("wins")      != std::string::npos ||
                 ctx.messageLine.find("Draw")       != std::string::npos ||
                 ctx.messageLine.find("draw")       != std::string::npos ||
                 ctx.messageLine.find("Stalemate")  != std::string::npos ||
                 ctx.messageLine.find("Checkmate")  != std::string::npos ||
                 ctx.messageLine.find("resigns")    != std::string::npos);
    if (!over) return;

    fillRectAlpha({0, 0, kBoardW, kBoardH}, {0, 0, 0, 255}, 175);

    SDL_Color gold   = {255, 215, 0,   255};
    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color silver = {180, 180, 180, 255};

    renderText(ctx.messageLine, fontBig_, gold,   kBoardW / 2, kBoardH / 2 - 60);
    renderText("R — Restart",   fontMed_, silver, kBoardW / 2, kBoardH / 2 + 20);
    renderText("T — Theme",     fontMed_, silver, kBoardW / 2, kBoardH / 2 + 50);
    renderText("ESC — Quit",    fontMed_, silver, kBoardW / 2, kBoardH / 2 + 80);
}

void SDL2Renderer::drawStatus(const RenderContext& ctx) {
    drawStatusBar(ctx);
}

void SDL2Renderer::drawStatusBar(const RenderContext& ctx) {
    // Background strip
    SDL_SetRenderDrawColor(renderer_, 20, 20, 20, 255);
    SDL_Rect bar = {0, kBoardH, kWinW, kStatusH};
    SDL_RenderFillRect(renderer_, &bar);

    // Turn indicator dot
    bool whiteTurn = ctx.state && ctx.state->activePlayer == Color::White;
    SDL_Color dot = whiteTurn ? SDL_Color{255,255,255,255} : SDL_Color{40,40,40,255};
    SDL_SetRenderDrawColor(renderer_, dot.r, dot.g, dot.b, 255);
    fillCircle(20, kBoardH + kStatusH / 2, 10, dot);
    if (!whiteTurn) {
        // Outline for black piece
        SDL_SetRenderDrawColor(renderer_, 150, 150, 150, 255);
        for (int r = 9; r <= 11; ++r) {
            SDL_Rect ring = {20 - r, kBoardH + kStatusH/2 - r, 2*r, 2*r};
            SDL_RenderDrawRect(renderer_, &ring);
        }
    }

    SDL_Color textColor = {220, 220, 220, 255};
    SDL_Color dimColor  = {130, 130, 130, 255};

    // Status text
    if (!ctx.statusLine.empty())
        renderTextLeft(ctx.statusLine, fontMed_, textColor, 40, kBoardH + 8);

    // Theme name + controls hint
    std::string hint = std::string("Theme: ") + kPalettes[themeIdx_].name
                     + "  |  T=theme  R=restart  U=undo  F=flip  ESC=quit";
    renderTextLeft(hint, fontMed_, dimColor, 40, kBoardH + 32);
}

// ─── Promotion picker ─────────────────────────────────────────────────────────
PieceTypeID SDL2Renderer::showPromotionPicker(Color color) {
    using namespace angry_chess;

    // 4 choices laid horizontally in centre of board
    static const PieceTypeID choices[]  = {QUEEN, ROOK, BISHOP, KNIGHT};
    static const char*        labels[]  = {"♕ Queen", "♖ Rook", "♗ Bishop", "♘ Knight"};
    static const char*        lblsBlack[]= {"♛ Queen", "♜ Rook", "♝ Bishop", "♞ Knight"};
    bool isWhite = (color == Color::White);

    int panelW = 600, panelH = 120;
    int panelX = (kWinW - panelW) / 2;
    int panelY = (kBoardH - panelH) / 2;

    SDL_Event ev;
    while (true) {
        // Draw panel
        fillRectAlpha({0, 0, kWinW, kBoardH}, {0, 0, 0, 255}, 160);
        fillRectAlpha({panelX, panelY, panelW, panelH}, {50, 50, 50, 255}, 240);

        SDL_SetRenderDrawColor(renderer_, 200, 200, 200, 255);
        SDL_Rect border = {panelX, panelY, panelW, panelH};
        SDL_RenderDrawRect(renderer_, &border);

        renderText("Promote to:", fontMed_, {255,255,255,255},
                   kWinW / 2, panelY - 25);

        for (int i = 0; i < 4; ++i) {
            int bx = panelX + 10 + i * 148;
            int by = panelY + 10;
            SDL_Rect btn = {bx, by, 140, panelH - 20};
            SDL_SetRenderDrawColor(renderer_, 80, 80, 80, 255);
            SDL_RenderFillRect(renderer_, &btn);
            SDL_SetRenderDrawColor(renderer_, 150, 150, 150, 255);
            SDL_RenderDrawRect(renderer_, &btn);
            const char* lbl = isWhite ? labels[i] : lblsBlack[i];
            renderText(lbl, fontMed_, {255, 255, 255, 255},
                       bx + 70, by + (panelH - 20) / 2);
        }
        SDL_RenderPresent(renderer_);

        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_MOUSEBUTTONDOWN) {
                int mx = ev.button.x, my = ev.button.y;
                if (my >= panelY + 10 && my <= panelY + panelH - 10) {
                    for (int i = 0; i < 4; ++i) {
                        int bx = panelX + 10 + i * 148;
                        if (mx >= bx && mx <= bx + 140)
                            return choices[i];
                    }
                }
            }
            if (ev.type == SDL_KEYDOWN) {
                switch (ev.key.keysym.sym) {
                    case SDLK_q: case SDLK_1: return QUEEN;
                    case SDLK_r: case SDLK_2: return ROOK;
                    case SDLK_b: case SDLK_3: return BISHOP;
                    case SDLK_n: case SDLK_4: return KNIGHT;
                    default: break;
                }
            }
            if (ev.type == SDL_QUIT) return QUEEN;
        }
    }
}

} // namespace bge
