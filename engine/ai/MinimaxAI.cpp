#include "engine/ai/MinimaxAI.hpp"
#include "engine/rules/IGameRules.hpp"
#include <chrono>
#include <algorithm>
#include <limits>

namespace bge {

static constexpr int kInfinity = std::numeric_limits<int>::max() / 2;

Move MinimaxAI::selectMove(const BoardState& state,
                            const IGameRules& rules,
                            Color forPlayer,
                            std::chrono::milliseconds timeLimit) {
    nodesSearched_ = 0;
    auto moves = rules.generateAllLegalMoves(state, forPlayer);
    if (moves.empty()) return {};

    bool maximizing = (forPlayer == Color::White);
    Move bestMove   = moves.front();
    int  bestScore  = maximizing ? -kInfinity : kInfinity;

    // Iterative deepening: try increasing depths until time runs out
    auto deadline = std::chrono::steady_clock::now() + timeLimit;

    for (int depth = 1; depth <= maxDepth_; ++depth) {
        Move  iterBest  = moves.front();
        int   iterScore = maximizing ? -kInfinity : kInfinity;

        for (const auto& m : moves) {
            BoardState next = rules.applyMove(state, m);
            int score = alphaBeta(next, rules,
                                  depth - 1,
                                  -kInfinity, kInfinity,
                                  !maximizing);
            if (maximizing ? score > iterScore : score < iterScore) {
                iterScore = score;
                iterBest  = m;
            }
        }
        bestMove  = iterBest;
        bestScore = iterScore;

        if (std::chrono::steady_clock::now() >= deadline) break;
    }

    return bestMove;
}

int MinimaxAI::alphaBeta(const BoardState& state,
                          const IGameRules& rules,
                          int depth, int alpha, int beta,
                          bool maximizing) {
    ++nodesSearched_;

    if (depth == 0) return evaluator_(state);

    auto moves = rules.generateAllLegalMoves(
        state, maximizing ? Color::White : Color::Black);

    if (moves.empty()) {
        // Terminal node — check if it's checkmate or stalemate
        return evaluator_(state);
    }

    if (maximizing) {
        int best = -kInfinity;
        for (const auto& m : moves) {
            BoardState next = rules.applyMove(state, m);
            best = std::max(best, alphaBeta(next, rules, depth - 1, alpha, beta, false));
            alpha = std::max(alpha, best);
            if (beta <= alpha) break; // β cut-off
        }
        return best;
    } else {
        int best = kInfinity;
        for (const auto& m : moves) {
            BoardState next = rules.applyMove(state, m);
            best = std::min(best, alphaBeta(next, rules, depth - 1, alpha, beta, true));
            beta = std::min(beta, best);
            if (beta <= alpha) break; // α cut-off
        }
        return best;
    }
}

} // namespace bge
