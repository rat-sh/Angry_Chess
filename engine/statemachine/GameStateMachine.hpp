#pragma once
#include "engine/statemachine/IGameState.hpp"
#include <memory>
#include <unordered_map>

namespace bge {

/// Simple push-down state machine.
/// States are registered by ID; transitions call onExit/onEnter.
class GameStateMachine {
public:
    /// Register a state implementation.
    void addState(GameStateID id, std::unique_ptr<IGameState> state);

    /// Transition to a new state (calls onExit on current, onEnter on next).
    void transitionTo(GameStateID id, GameEngine& engine);

    /// Update the current state.
    void update(GameEngine& engine, float dt);

    [[nodiscard]] GameStateID current() const noexcept { return currentID_; }

    [[nodiscard]] bool is(GameStateID id) const noexcept { return currentID_ == id; }

private:
    std::unordered_map<GameStateID, std::unique_ptr<IGameState>> states_;
    IGameState* currentState_{nullptr};
    GameStateID currentID_{GameStateID::Menu};
};

} // namespace bge
