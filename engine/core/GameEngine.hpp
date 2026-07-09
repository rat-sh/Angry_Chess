#pragma once
#include "engine/board/BoardState.hpp"
#include "engine/rules/IGameRules.hpp"
#include "engine/renderer/IRenderer.hpp"
#include "engine/input/IInputHandler.hpp"
#include "engine/ai/IAIPlayer.hpp"
#include "engine/theme/ITheme.hpp"
#include "engine/core/EventBus.hpp"
#include "engine/statemachine/GameStateMachine.hpp"
#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace bge {

/// Configuration for a single game session.
struct GameConfig {
    std::string gameName    {"angry_chess"};
    std::string themeName   {"angry_classic"};
    bool        whiteIsAI   {false};
    bool        blackIsAI   {true};
    std::string whiteAIName {"minimax"};
    std::string blackAIName {"minimax"};
    bool        flipped     {false};
    std::chrono::milliseconds aiTimeLimit{500};
};

/// The central orchestrator.
/// Owns all subsystems and drives the main game loop.
/// Has zero knowledge of chess rules — everything flows through interfaces.
class GameEngine {
public:
    explicit GameEngine(std::unique_ptr<IGameRules>    rules,
                        std::unique_ptr<IRenderer>     renderer,
                        std::unique_ptr<IInputHandler> input,
                        std::unique_ptr<ITheme>        theme,
                        GameConfig                     config = {});

    // ── Entry point ──────────────────────────────────────────────────────────
    /// Start and run the game loop until the game ends or player quits.
    void run();

    // ── Subsystem accessors (used by states) ─────────────────────────────────
    IGameRules&    rules()    { return *rules_;    }
    IRenderer&     renderer() { return *renderer_; }
    ITheme&        theme()    { return *theme_;    }
    IInputHandler& input()    { return *input_;    }

    BoardState&       boardState()       { return state_; }
    const BoardState& boardState() const { return state_; }

    // ── AI ───────────────────────────────────────────────────────────────────
    void setWhiteAI(std::unique_ptr<IAIPlayer> ai) { whiteAI_ = std::move(ai); }
    void setBlackAI(std::unique_ptr<IAIPlayer> ai) { blackAI_ = std::move(ai); }

    // ── Render ───────────────────────────────────────────────────────────────
    void render();

    // ── Public game commands (called from run loop) ───────────────────────────
    void undo();
    void resign(Color color);
    void flipBoard();
    void changeTheme(std::unique_ptr<ITheme> newTheme);
    void restartGame(); ///< Reset board to initial state, clear history


    // ── Event buses (subscribe from outside) ──────────────────────────────────
    EventBus<MoveMadeEvent>&    onMoveMade()   { return moveBus_;    }
    EventBus<GameOverEvent>&    onGameOver()   { return gameOverBus_;}
    EventBus<CheckEvent>&       onCheck()      { return checkBus_;   }
    EventBus<TurnChangedEvent>& onTurnChanged(){ return turnBus_;    }

    void stop() { running_ = false; }

private:
    // ── Subsystems ────────────────────────────────────────────────────────────
    std::unique_ptr<IGameRules>    rules_;
    std::unique_ptr<IRenderer>     renderer_;
    std::unique_ptr<IInputHandler> input_;
    std::unique_ptr<ITheme>        theme_;
    std::unique_ptr<IAIPlayer>     whiteAI_;
    std::unique_ptr<IAIPlayer>     blackAI_;

    // ── State ─────────────────────────────────────────────────────────────────
    BoardState               state_;
    GameConfig               config_;
    std::vector<BoardState>  history_;      // for undo
    std::optional<Move>      lastMove_;
    std::vector<SquareHighlight> highlights_;
    std::string              statusLine_;
    std::string              messageLine_;
    bool                     running_{true};
    GameStatus               gameStatus_{GameStatus::Ongoing};

    // ── Event buses ───────────────────────────────────────────────────────────
    EventBus<MoveMadeEvent>    moveBus_;
    EventBus<GameOverEvent>    gameOverBus_;
    EventBus<CheckEvent>       checkBus_;
    EventBus<TurnChangedEvent> turnBus_;

    // ── Internal helpers ──────────────────────────────────────────────────────
    void processEvent(const InputEvent& ev);
    void applyPlayerMove(const Move& raw);
    void runAITurn();
    void afterMove(const Move& applied);
    void updateStatus();
    void buildHighlights(Position selected, const std::vector<Move>& legalMoves);
    void showHelp();
    void printHint();
    RenderContext makeRenderContext() const;

    [[nodiscard]] bool isAITurn() const;
    [[nodiscard]] IAIPlayer* activeAI() const;
};

} // namespace bge
