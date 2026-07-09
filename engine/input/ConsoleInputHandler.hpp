#pragma once
#include "engine/input/IInputHandler.hpp"
#include <string>

namespace bge {

/// Reads move input from stdin.
/// Accepts formats: "e2e4", "e2 e4", "e2-e4", or commands like "quit", "undo", "hint".
class ConsoleInputHandler final : public IInputHandler {
public:
    ConsoleInputHandler() = default;

    [[nodiscard]] std::optional<InputEvent> poll() override;
    [[nodiscard]] InputEvent waitForInput() override;

private:
    [[nodiscard]] InputEvent parseLine(const std::string& line) const;
};

} // namespace bge
