#pragma once
#include "engine/input/IInputHandler.hpp"
#include "engine/renderer/SDL2Renderer.hpp"
#include <SDL2/SDL.h>
#include <optional>

namespace bge {

/// SDL2-based input handler.
/// Translates SDL events into InputEvent variants consumed by GameEngine.
///
/// Two-click move flow:
///   First  click → select piece  → sends nothing (stored internally)
///   Second click → confirm dest  → sends MoveInputEvent(from, to)
///
/// Keyboard shortcuts:
///   T   → CommandEvent("theme")
///   R   → CommandEvent("restart")
///   U   → CommandEvent("undo")
///   F   → CommandEvent("flip")
///   H   → CommandEvent("hint")
///   ESC → QuitEvent
class SDL2InputHandler final : public IInputHandler {
public:
    explicit SDL2InputHandler(SDL2Renderer& renderer)
        : renderer_(renderer) {}

    /// Blocks until a meaningful event is ready, then returns it.
    [[nodiscard]] InputEvent waitForInput() override;

    /// Non-blocking: returns nullopt if no event pending.
    [[nodiscard]] std::optional<InputEvent> poll() override;

private:
    SDL2Renderer& renderer_;

    std::optional<Position> pendingFrom_;   // first-click selection
    bool flipped_{false};                   // kept in sync via flip commands

    Position pixelToPosition(int x, int y) const;
    bool isOnBoard(int x, int y) const;
};

} // namespace bge
