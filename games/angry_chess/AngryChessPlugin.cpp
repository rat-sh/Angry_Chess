#include "games/angry_chess/AngryChessPlugin.hpp"
#include "games/angry_chess/rules/ChessRules.hpp"
#include "games/angry_chess/ai/ChessEvaluator.hpp"
#include "games/angry_chess/theme/ClassicTheme.hpp"
#include "games/angry_chess/theme/AngryTheme.hpp"
#include "engine/ai/RandomAI.hpp"
#include "engine/ai/MinimaxAI.hpp"
#include "engine/renderer/ConsoleRenderer.hpp"
#include "engine/input/ConsoleInputHandler.hpp"

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
    registry.registerAI("random",
        []{ return std::make_unique<RandomAI>(); });

    registry.registerAI("minimax",
        []{
            // Inject the chess-specific evaluator into the generic minimax engine
            return std::make_unique<MinimaxAI>(chessEvaluate, /*maxDepth=*/4);
        });

    registry.registerAI("minimax_fast",
        []{
            return std::make_unique<MinimaxAI>(chessEvaluate, /*maxDepth=*/2);
        });

    // ── Renderer ─────────────────────────────────────────────────────────────
    registry.registerRenderer("console",
        []{ return std::make_unique<ConsoleRenderer>(); });

    // ── Input handler ─────────────────────────────────────────────────────────
    registry.registerInputHandler("console",
        []{ return std::make_unique<ConsoleInputHandler>(); });
}

} // namespace angry_chess
