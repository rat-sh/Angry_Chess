#pragma once
#include "engine/ai/IAIPlayer.hpp"
#include <functional>
#include <limits>

namespace bge {

/// Evaluator function: positive = good for White, negative = good for Black.
using EvaluatorFn = std::function<int(const BoardState& state)>;

/// Alpha-beta pruning minimax AI with iterative deepening.
/// The evaluator is injected, making this fully game-agnostic.
/// The chess plugin provides ChessEvaluator; any game can provide its own.
class MinimaxAI final : public IAIPlayer {
public:
    explicit MinimaxAI(EvaluatorFn evaluator, int maxDepth = 4)
        : evaluator_(std::move(evaluator)), maxDepth_(maxDepth) {}

    Move selectMove(const BoardState& state,
                    const IGameRules& rules,
                    Color forPlayer,
                    std::chrono::milliseconds timeLimit) override;

    void reset() override { nodesSearched_ = 0; }
    std::string_view name() const override { return "Minimax (Alpha-Beta)"; }

    [[nodiscard]] uint64_t nodesSearched() const noexcept { return nodesSearched_; }
    void setMaxDepth(int d) noexcept { maxDepth_ = d; }

private:
    EvaluatorFn evaluator_;
    int         maxDepth_;
    uint64_t    nodesSearched_{0};

    int alphaBeta(const BoardState& state,
                  const IGameRules& rules,
                  int depth, int alpha, int beta,
                  bool maximizing);
};

} // namespace bge
