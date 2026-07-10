/// sdl_main.cpp — SDL2 graphical entry point for Angry Chess Engine
///
/// Architecture:
///   SDL2Renderer  implements IRenderer → plugged into GameEngine
///   SDL2InputHandler implements IInputHandler → plugged into GameEngine
///   GameEngine::run() blocks and drives the full game loop
///   R=restart  T=theme  U=undo  F=flip  ESC=quit

#include "engine/core/GameEngine.hpp"
#include "engine/core/PluginRegistry.hpp"
#include "engine/renderer/SDL2Renderer.hpp"
#include "engine/input/SDL2InputHandler.hpp"
#include "chess/AngryChessPlugin.hpp"
#include "chess/FenParser.hpp"
#include <iostream>
#include <string>
#include <stdexcept>

// ─── CLI ──────────────────────────────────────────────────────────────────────
struct CliArgs { std::string fen; std::string pgn; };
static CliArgs parseCli(int argc, char** argv) {
    CliArgs a;
    for (int i = 1; i < argc; ++i) {
        std::string s = argv[i];
        if ((s == "--fen" || s == "-f") && i+1 < argc) a.fen = argv[++i];
        if ((s == "--pgn" || s == "-p") && i+1 < argc) a.pgn = argv[++i];
    }
    return a;
}

// ─── Helpers ──────────────────────────────────────────────────────────────────
static void drawMenuText(SDL_Renderer* rend, TTF_Font* font,
                         const char* text, SDL_Color col, int cx, int cy) {
    SDL_Surface* s = TTF_RenderUTF8_Blended(font, text, col);
    if (!s) return;
    SDL_Texture* t = SDL_CreateTextureFromSurface(rend, s);
    SDL_Rect r = { cx - s->w/2, cy - s->h/2, s->w, s->h };
    SDL_FreeSurface(s);
    if (t) { SDL_RenderCopy(rend, t, nullptr, &r); SDL_DestroyTexture(t); }
}

// ─── Mode selection menu ──────────────────────────────────────────────────────
/// Returns 1..4 (mode) or 0 (quit)
static int showMenu(SDL_Window* win, SDL_Renderer* rend, bge::SDL2Renderer& gfx) {
    TTF_Font* big = TTF_OpenFont(
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 40);
    TTF_Font* med = TTF_OpenFont(
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 22);
    if (!big || !med) { TTF_CloseFont(big); TTF_CloseFont(med); return 2; }

    const char* opts[4] = {
        "1.  Human  vs  Human",
        "2.  Human (White)  vs  Stockfish  (Black)",
        "3.  Stockfish  (White)  vs  Human  (Black)",
        "4.  Stockfish  vs  Stockfish  (spectate)",
    };

    int W, H;
    SDL_GetWindowSize(win, &W, &H);
    int selected = -1;

    SDL_Event ev;
    while (selected < 0) {
        SDL_SetRenderDrawColor(rend, 22, 22, 30, 255);
        SDL_RenderClear(rend);

        drawMenuText(rend, big, "♟  Angry  Chess",
                     {255, 215, 0, 255}, W/2, 90);
        drawMenuText(rend, med, "Select a game mode",
                     {160, 160, 160, 255}, W/2, 155);

        for (int i = 0; i < 4; ++i) {
            int y = 230 + i * 75;
            SDL_Rect btn = { 70, y - 26, W - 140, 54 };
            SDL_SetRenderDrawColor(rend, 45, 45, 60, 255);
            SDL_RenderFillRect(rend, &btn);
            SDL_SetRenderDrawColor(rend, 80, 80, 110, 255);
            SDL_RenderDrawRect(rend, &btn);
            drawMenuText(rend, med, opts[i],
                         {220, 220, 255, 255}, W/2, y + 1);
        }

        drawMenuText(rend, med,
                     "T = theme  |  R = restart in game  |  ESC = quit",
                     {100, 100, 120, 255}, W/2, H - 50);

        SDL_RenderPresent(rend);

        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) { selected = 0; break; }
            if (ev.type == SDL_KEYDOWN) {
                switch (ev.key.keysym.sym) {
                    case SDLK_1: selected = 1; break;
                    case SDLK_2: selected = 2; break;
                    case SDLK_3: selected = 3; break;
                    case SDLK_4: selected = 4; break;
                    case SDLK_ESCAPE: selected = 0; break;
                    case SDLK_t: gfx.nextTheme(); break;
                    default: break;
                }
            }
            if (ev.type == SDL_MOUSEBUTTONDOWN) {
                int my = ev.button.y;
                for (int i = 0; i < 4; ++i) {
                    int y = 230 + i * 75;
                    if (my >= y - 26 && my <= y + 28) {
                        selected = i + 1; break;
                    }
                }
            }
        }
    }
    TTF_CloseFont(big);
    TTF_CloseFont(med);
    return selected;
}

// ─── IRenderer wrapper (non-owning, delegates to SDL2Renderer) ────────────────
namespace bge {
struct RefRenderer final : public IRenderer {
    SDL2Renderer& r;
    explicit RefRenderer(SDL2Renderer& rr) : r(rr) {}
    void beginFrame() override                    { r.beginFrame(); }
    void drawBoard(const RenderContext& c) override{ r.drawBoard(c); }
    void drawStatus(const RenderContext& c) override{ r.drawStatus(c); }
    void drawPrompt(std::string_view p)   override { r.drawPrompt(p); }
    void endFrame()   override                    { r.endFrame(); }
};
} // namespace bge

// ─── main ─────────────────────────────────────────────────────────────────────
int main(int argc, char** argv) {
    try {
        CliArgs cli = parseCli(argc, argv);

        // Plugin registry
        bge::PluginRegistry registry;
        angry_chess::AngryChessPlugin::install(registry);

        // Single SDL2Renderer owns the window for the whole process lifetime
        bge::SDL2Renderer gfx;

        // Outer loop: menu → game → restart
        while (gfx.isOpen()) {
            // ── 1. Show mode selection menu ───────────────────────────────────
            int mode = showMenu(gfx.window(), gfx.renderer(), gfx);
            if (mode == 0) break; // user pressed ESC

            bool whiteIsAI = (mode == 3 || mode == 4);
            bool blackIsAI = (mode == 2 || mode == 4);

            // ── 2. Build GameEngine ───────────────────────────────────────────
            bge::GameConfig cfg;
            cfg.gameName    = "angry_chess";
            cfg.themeName   = "classic";
            cfg.whiteIsAI   = whiteIsAI;
            cfg.blackIsAI   = blackIsAI;
            cfg.whiteAIName = (mode == 4) ? "random" : "stockfish";
            cfg.blackAIName = (mode == 4) ? "random" : "stockfish";
            cfg.aiTimeLimit = std::chrono::milliseconds{1000};
            // 600ms pacing delay for AI vs AI spectator mode
            cfg.aiMoveDelay = (mode == 4)
                              ? std::chrono::milliseconds{600}
                              : std::chrono::milliseconds{0};

            auto sdlInput = std::make_unique<bge::SDL2InputHandler>(gfx);
            auto rendWrap = std::make_unique<bge::RefRenderer>(gfx);

            bge::GameEngine engine(
                registry.createGame("angry_chess"),
                std::move(rendWrap),
                std::move(sdlInput),
                registry.createTheme("classic"),
                cfg
            );

            // Custom FEN starting position
            if (!cli.fen.empty()) {
                try {
                    engine.boardState() = bge::FenParser::parse(cli.fen);
                } catch (std::exception& ex) {
                    std::cerr << "[FEN] " << ex.what() << "\n";
                }
            }

            // AI players — Stockfish with minimax fallback
            if (whiteIsAI) engine.setWhiteAI(registry.createAI(cfg.whiteAIName));
            if (blackIsAI) engine.setBlackAI(registry.createAI(cfg.blackAIName));

            // Theme change hook — T key handled in SDL2InputHandler returns
            // CommandEvent("theme") which GameEngine forwards to processEvent.
            // We intercept via the renderer proxy:
            // (already handled in processEvent via "theme" → gfx.nextTheme()
            //  BUT processEvent doesn't know about gfx. So we subscribe.)
            // For theme: SDL2InputHandler returns CommandEvent{"theme"} → engine
            // calls processEvent → no built-in handler, falls to "unknown".
            // Fix: we intercept in onMoveMade/subscriber. Actually the cleanest
            // fix is to hook theme in SDL2InputHandler before returning the event:
            // (SDL2InputHandler already calls gfx.nextTheme() internally for T)
            // → done via injected ref in SDL2InputHandler.

            // ── 3. Run (blocks until ESC/quit or R/restart) ───────────────────
            engine.run();

            // After run() exits: if window still open → back to menu (restart)
            // If window was closed → exit outer loop
        }

        return 0;

    } catch (std::exception& e) {
        std::cerr << "\n[FATAL] " << e.what() << "\n";
        return 1;
    }
}
