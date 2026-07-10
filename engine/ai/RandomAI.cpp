#include "engine/ai/RandomAI.hpp"
#include "engine/rules/IGameRules.hpp"
#include <random>

namespace bge {

Move RandomAI::selectMove(const BoardState& state,
                           const IGameRules& rules,
                           Color forPlayer,
                           std::chrono::milliseconds) {
    auto moves = rules.generateAllLegalMoves(state, forPlayer);
    if (moves.empty()) return {};

    std::mt19937 rng(seed_++);
    std::uniform_int_distribution<std::size_t> dist(0, moves.size() - 1);
    return moves[dist(rng)];
}

} // namespace bge
