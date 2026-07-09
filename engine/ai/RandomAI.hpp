#pragma once
#include "engine/ai/IAIPlayer.hpp"

namespace bge {

/// Selects a uniformly-random legal move.
/// Used as baseline / fallback AI and for testing.
class RandomAI final : public IAIPlayer {
public:
    explicit RandomAI(unsigned seed = 42) : seed_(seed) {}

    Move selectMove(const BoardState& state,
                    const IGameRules& rules,
                    Color forPlayer,
                    std::chrono::milliseconds) override;

    void reset() override { seed_ = 42; }
    std::string_view name() const override { return "Random"; }

private:
    unsigned seed_;
};

} // namespace bge
