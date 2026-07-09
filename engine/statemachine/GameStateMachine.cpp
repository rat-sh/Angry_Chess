#include "engine/statemachine/GameStateMachine.hpp"
#include <stdexcept>

namespace bge {

void GameStateMachine::addState(GameStateID id, std::unique_ptr<IGameState> state) {
    states_[id] = std::move(state);
}

void GameStateMachine::transitionTo(GameStateID id, GameEngine& engine) {
    if (currentState_) currentState_->onExit(engine);

    auto it = states_.find(id);
    if (it == states_.end())
        throw std::runtime_error("State not registered");

    currentID_    = id;
    currentState_ = it->second.get();
    currentState_->onEnter(engine);
}

void GameStateMachine::update(GameEngine& engine, float dt) {
    if (currentState_) currentState_->onUpdate(engine, dt);
}

} // namespace bge
