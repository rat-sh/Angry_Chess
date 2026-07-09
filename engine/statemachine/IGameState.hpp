#pragma once
#include "engine/statemachine/States.hpp"
#include <memory>

namespace bge {

/// Forward declaration for the context the state can query.
class GameEngine;

/// Interface for a single game state (Menu, Playing, Paused, GameOver).
class IGameState {
public:
    virtual ~IGameState() = default;

    /// Called once when entering this state.
    virtual void onEnter(GameEngine& engine) {}

    /// Called every frame tick.
    virtual void onUpdate(GameEngine& engine, float dt) {}

    /// Called once when leaving this state.
    virtual void onExit(GameEngine& engine) {}

    [[nodiscard]] virtual GameStateID stateID() const = 0;
};

} // namespace bge
