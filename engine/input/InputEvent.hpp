#pragma once
#include "engine/rules/Move.hpp"
#include <string>
#include <variant>
#include <optional>

namespace bge {

// ─── Input event types ────────────────────────────────────────────────────────

/// Player selected a square on the board.
struct SquareSelectedEvent {
    Position pos;
};

/// Player confirmed a move (e.g. typed "e2e4" and pressed Enter).
struct MoveInputEvent {
    Move move; ///< Partially filled move; rules will fully annotate it.
};

/// Player typed a command (e.g. "quit", "undo", "hint", "theme classic").
struct CommandEvent {
    std::string command;
    std::string argument; ///< optional argument after the command
};

/// EOF / quit signal from the input source.
struct QuitEvent {};

// ─── InputEvent variant ──────────────────────────────────────────────────────
using InputEvent = std::variant<
    SquareSelectedEvent,
    MoveInputEvent,
    CommandEvent,
    QuitEvent
>;

/// Helpers
inline bool isQuit(const InputEvent& e) noexcept {
    return std::holds_alternative<QuitEvent>(e);
}
inline bool isMoveInput(const InputEvent& e) noexcept {
    return std::holds_alternative<MoveInputEvent>(e);
}

} // namespace bge
