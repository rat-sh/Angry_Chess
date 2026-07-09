#pragma once
#include "engine/input/InputEvent.hpp"
#include <optional>

namespace bge {

/// Non-blocking input source interface.
/// Concrete implementations: ConsoleInputHandler, SDL2InputHandler, NetworkInputHandler.
class IInputHandler {
public:
    virtual ~IInputHandler() = default;

    /// Poll for the next event.  Returns nullopt when no input is ready.
    /// Callers should loop on this during the game update tick.
    [[nodiscard]] virtual std::optional<InputEvent> poll() = 0;

    /// Blocking wait — returns when at least one event is available.
    /// Default implementation calls poll() in a tight loop; override for efficiency.
    [[nodiscard]] virtual InputEvent waitForInput();
};

} // namespace bge
