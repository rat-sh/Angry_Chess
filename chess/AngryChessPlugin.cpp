#include "chess/AngryChessPlugin.hpp"
#include "chess/rules/ChessRules.hpp"
#include "chess/ai/StockfishAI/StockfishAI.hpp"
#include "chess/theme/ClassicTheme.hpp"
#include "chess/theme/AngryTheme.hpp"
#include "engine/ai/RandomAI.hpp"
#include "engine/renderer/ConsoleRenderer.hpp"
#include "engine/input/ConsoleInputHandler.hpp"
#include <iostream>

namespace angry_chess {
using namespace bge;

void AngryChessPlugin::install(PluginRegistry& registry) {
    // ── Game ruleset ──────────────────────────────────────────────────────────
    registry.registerGame("angry_chess",
        []{ return std::make_unique<ChessRules>(); });

    // ── Themes ────────────────────────────────────────────────────────────────
    registry.registerTheme("classic",
        []{ return std::make_unique<ClassicTheme>(); });

    registry.registerTheme("angry",
        []{ return std::make_unique<AngryTheme>(); });

    // ── AI players ────────────────────────────────────────────────────────────

    // Built-in AI: picks random legal moves (no external dependencies).
    registry.registerAI("random",
        []{ return std::make_unique<RandomAI>(); });

    // Stockfish: real chess engine via UCI protocol (falls back to random).
    registry.registerAI("stockfish", []() -> std::unique_ptr<IAIPlayer> {
        try {
            return std::make_unique<StockfishAI>("stockfish");
        } catch (const std::exception& ex) {
            std::cerr << "\033[33m[AI] Stockfish not available: " << ex.what()
                      << " — using Random AI instead.\033[0m\n";
            return std::make_unique<RandomAI>();
        }
    });

    // ── Renderer ─────────────────────────────────────────────────────────────
    registry.registerRenderer("console",
        []{ return std::make_unique<ConsoleRenderer>(); });

    // ── Input handler ─────────────────────────────────────────────────────────
    registry.registerInputHandler("console",
        []{ return std::make_unique<ConsoleInputHandler>(); });
}

} // namespace angry_chess
