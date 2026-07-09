#include "engine/core/GameEngine.hpp"
#include "engine/rules/IGameRules.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdio>

namespace bge {

// ─── Constructor ──────────────────────────────────────────────────────────────

GameEngine::GameEngine(std::unique_ptr<IGameRules>    rules,
                       std::unique_ptr<IRenderer>     renderer,
                       std::unique_ptr<IInputHandler> input,
                       std::unique_ptr<ITheme>        theme,
                       GameConfig                     config)
    : rules_   (std::move(rules))
    , renderer_(std::move(renderer))
    , input_   (std::move(input))
    , theme_   (std::move(theme))
    , config_  (std::move(config))
{
    // Initialise board
    state_ = rules_->initialState();
    updateStatus();
}

// ─── Main Loop ────────────────────────────────────────────────────────────────

void GameEngine::run() {
    running_ = true;

    while (running_) {
        render();

        if (gameStatus_ != GameStatus::Ongoing &&
            gameStatus_ != GameStatus::Check) {
            // Game over — wait for quit/undo
            InputEvent ev = input_->waitForInput();
            processEvent(ev);
            continue;
        }

        // AI turn?
        if (isAITurn()) {
            runAITurn();
            continue;
        }

        // Human turn — show prompt and read input
        std::string promptText;
        promptText  = std::string(colorName(state_.activePlayer));
        promptText += "'s move (e.g. e2e4): ";
        renderer_->drawPrompt(promptText);

        InputEvent ev = input_->waitForInput();
        processEvent(ev);
    }
}

// ─── Event Processing ─────────────────────────────────────────────────────────

void GameEngine::processEvent(const InputEvent& ev) {
    std::visit([this](auto&& e) {
        using T = std::decay_t<decltype(e)>;

        if constexpr (std::is_same_v<T, MoveInputEvent>) {
            applyPlayerMove(e.move);

        } else if constexpr (std::is_same_v<T, SquareSelectedEvent>) {
            // SDL2 click — select piece and show highlights
            Position pos = e.pos;
            if (!state_.isEmpty(pos) && state_.colorAt(pos) == state_.activePlayer) {
                auto legalMoves = rules_->generateLegalMoves(state_, pos);
                buildHighlights(pos, legalMoves);
            } else {
                highlights_.clear();
            }

        } else if constexpr (std::is_same_v<T, CommandEvent>) {
            if      (e.command == "quit")    { running_ = false; }
            else if (e.command == "undo")    { undo(); }
            else if (e.command == "flip")    { flipBoard(); }
            else if (e.command == "resign")  { resign(state_.activePlayer); }
            else if (e.command == "hint")    { printHint(); }
            else if (e.command == "help")    { showHelp(); }
            else if (e.command == "restart") { restartGame(); }
            else if (e.command == "theme")   { /* SDL2Renderer handles it directly */ }
            else if (e.command == "deselect") { highlights_.clear(); }
            else if (e.command == "select") {
                // argument is "rank file"
                int r = 0, f = 0;
                if (std::sscanf(e.argument.c_str(), "%d %d", &r, &f) == 2) {
                    Position pos{static_cast<int8_t>(r), static_cast<int8_t>(f)};
                    if (!state_.isEmpty(pos) && state_.colorAt(pos) == state_.activePlayer) {
                        auto legalMoves = rules_->generateLegalMoves(state_, pos);
                        buildHighlights(pos, legalMoves);
                    } else {
                        highlights_.clear();
                    }
                }
            }
            else if (e.command == "unknown") {
                messageLine_ = "Unknown command. Type 'help' for options.";
            }

        } else if constexpr (std::is_same_v<T, QuitEvent>) {
            running_ = false;
        }
    }, ev);
}

// ─── Move Handling ────────────────────────────────────────────────────────────

void GameEngine::applyPlayerMove(const Move& raw) {
    MoveResult result = rules_->validateMove(state_, raw);

    if (!isLegal(result)) {
        messageLine_ = std::string("Invalid move: ") + toString(result);
        return;
    }

    // Push to history for undo
    history_.push_back(state_);

    // Apply through rules (allows game-specific side-effects)
    Move applied = raw;
    // Find the fully annotated move from legal moves
    auto legalMoves = rules_->generateLegalMoves(state_, raw.from);
    for (const auto& lm : legalMoves) {
        if (lm.from == raw.from && lm.to == raw.to) {
            // Use promotion type from user input if present
            applied = lm;
            if (raw.promotionType != kNoPiece) applied.promotionType = raw.promotionType;
            break;
        }
    }

    state_ = rules_->applyMove(state_, applied);
    afterMove(applied);
}

void GameEngine::runAITurn() {
    IAIPlayer* ai = activeAI();
    if (!ai) return;

    messageLine_ = std::string(ai->name()) + " is thinking...";
    render();

    Move m = ai->selectMove(state_, *rules_, state_.activePlayer, config_.aiTimeLimit);
    if (!m.isValid()) {
        messageLine_ = "AI has no legal moves.";
        return;
    }

    history_.push_back(state_);
    state_ = rules_->applyMove(state_, m);
    afterMove(m);
}

void GameEngine::afterMove(const Move& applied) {
    lastMove_ = applied;
    highlights_.clear();

    // Emit MoveMade event
    moveBus_.publish(MoveMadeEvent{
        {applied.from.rank, applied.from.file},
        {applied.to.rank,   applied.to.file},
        state_.typeAt(applied.to),
        static_cast<int8_t>(state_.activePlayer == Color::White ? -1 : 1)
    });

    // Check game status
    GameStatus newStatus = rules_->checkGameStatus(state_, state_.activePlayer);
    gameStatus_ = newStatus;

    if (newStatus == GameStatus::Check) {
        checkBus_.publish(CheckEvent{static_cast<int8_t>(state_.activePlayer)});
        messageLine_ = theme_->tauntOnCheck().empty()
                     ? "Check!"
                     : theme_->tauntOnCheck();
    } else if (newStatus == GameStatus::Checkmate) {
        Color winner = opposite(state_.activePlayer);
        messageLine_ = theme_->tauntOnCheckmate().empty()
                     ? std::string(colorName(winner)) + " wins by checkmate!"
                     : theme_->tauntOnCheckmate();
        gameOverBus_.publish(GameOverEvent{
            static_cast<int8_t>(winner),
            static_cast<uint8_t>(GameStatus::Checkmate)
        });
    } else if (newStatus == GameStatus::Stalemate) {
        messageLine_ = "Stalemate! It's a draw.";
        gameOverBus_.publish(GameOverEvent{0, static_cast<uint8_t>(GameStatus::Stalemate)});
    } else if (newStatus == GameStatus::Draw) {
        messageLine_ = "Draw by fifty-move rule!";
        gameOverBus_.publish(GameOverEvent{0, static_cast<uint8_t>(GameStatus::Draw)});
    } else {
        messageLine_ = "";
    }

    turnBus_.publish(TurnChangedEvent{static_cast<int8_t>(state_.activePlayer)});
    updateStatus();
}

// ─── Undo ──────────────────────────────────────────────────────────────────────

void GameEngine::undo() {
    if (history_.size() < 1) {
        messageLine_ = "Nothing to undo.";
        return;
    }
    // Undo twice if it was an AI move (undo opponent + our own)
    std::size_t steps = (isAITurn() && history_.size() >= 2) ? 2 : 1;
    for (std::size_t i = 0; i < steps && !history_.empty(); ++i) {
        state_ = history_.back();
        history_.pop_back();
    }
    lastMove_    = std::nullopt;
    highlights_.clear();
    messageLine_ = "Move undone.";
    gameStatus_  = GameStatus::Ongoing;
    updateStatus();
}

// ─── Commands ─────────────────────────────────────────────────────────────────

void GameEngine::resign(Color color) {
    Color winner = opposite(color);
    messageLine_ = std::string(colorName(color)) + " resigns. " +
                   std::string(colorName(winner)) + " wins!";
    gameStatus_ = GameStatus::Resigned;
    gameOverBus_.publish(GameOverEvent{static_cast<int8_t>(winner),
                                       static_cast<uint8_t>(GameStatus::Resigned)});
}

void GameEngine::flipBoard() {
    config_.flipped = !config_.flipped;
    messageLine_    = "Board flipped.";
}

void GameEngine::restartGame() {
    state_       = rules_->initialState();
    history_.clear();
    highlights_.clear();
    lastMove_    = std::nullopt;
    messageLine_ = "Game restarted!";
    gameStatus_  = GameStatus::Ongoing;
    updateStatus();
}

void GameEngine::changeTheme(std::unique_ptr<ITheme> newTheme) {
    if (newTheme) {
        std::string nm{newTheme->name()};
        theme_       = std::move(newTheme);
        messageLine_ = "Theme changed to: " + nm;
    }
}

void GameEngine::showHelp() {
    messageLine_ =
        "Commands: e2e4 (move) | undo | hint | flip | resign | theme <name> | quit";
}

void GameEngine::printHint() {
    auto moves = rules_->generateAllLegalMoves(state_, state_.activePlayer);
    if (moves.empty()) {
        messageLine_ = "No legal moves available.";
        return;
    }
    // Pick first move as hint
    const auto& m = moves.front();
    messageLine_ = "Hint: " + m.from.toAlgebraic() + m.to.toAlgebraic();
}

// ─── Status / Render helpers ──────────────────────────────────────────────────

void GameEngine::updateStatus() {
    std::ostringstream ss;
    ss << colorName(state_.activePlayer) << "'s turn"
       << "  |  Move " << state_.fullMoveNumber;
    if (state_.halfMoveClock > 0)
        ss << "  |  50-move: " << state_.halfMoveClock << "/100";
    statusLine_ = ss.str();
}

RenderContext GameEngine::makeRenderContext() const {
    RenderContext ctx;
    ctx.state       = &state_;
    ctx.theme       = theme_.get();
    ctx.highlights  = highlights_;
    ctx.statusLine  = statusLine_;
    ctx.messageLine = messageLine_;
    ctx.lastMove    = lastMove_;
    ctx.flipped     = config_.flipped;

    // Highlight last move squares
    if (lastMove_) {
        ctx.highlights.push_back({lastMove_->from, HighlightType::LastMove});
        ctx.highlights.push_back({lastMove_->to,   HighlightType::LastMove});
    }
    return ctx;
}

void GameEngine::render() {
    renderer_->beginFrame();
    renderer_->drawBoard(makeRenderContext());
    renderer_->drawStatus(makeRenderContext());
    renderer_->endFrame();
}

// ─── Helpers ─────────────────────────────────────────────────────────────────

bool GameEngine::isAITurn() const {
    if (state_.activePlayer == Color::White && config_.whiteIsAI) return true;
    if (state_.activePlayer == Color::Black && config_.blackIsAI) return true;
    return false;
}

IAIPlayer* GameEngine::activeAI() const {
    if (state_.activePlayer == Color::White && config_.whiteIsAI) return whiteAI_.get();
    if (state_.activePlayer == Color::Black && config_.blackIsAI) return blackAI_.get();
    return nullptr;
}

void GameEngine::buildHighlights(Position selected, const std::vector<Move>& legalMoves) {
    highlights_.clear();
    highlights_.push_back({selected, HighlightType::Selected});
    for (const auto& m : legalMoves)
        highlights_.push_back({m.to, HighlightType::LegalMove});
}

} // namespace bge
