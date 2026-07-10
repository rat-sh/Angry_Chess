#include "engine/input/ConsoleInputHandler.hpp"
#include "engine/input/IInputHandler.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
// POSIX non-blocking stdin check
#include <sys/select.h>
#include <unistd.h>

namespace bge {

// Default implementation: busy-wait (concrete handlers override for efficiency)
InputEvent IInputHandler::waitForInput() {
    while (true) {
        auto ev = poll();
        if (ev.has_value()) return *ev;
    }
}

// ── ConsoleInputHandler ───────────────────────────────────────────────────────

std::optional<InputEvent> ConsoleInputHandler::poll() {
    // Non-blocking check: use select() with zero timeout to peek at stdin.
    // Returns nullopt immediately if no data is available (e.g. during AI turn).
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    struct timeval timeout{0, 0}; // zero timeout = non-blocking
    int ready = select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &timeout);

    if (ready <= 0) return std::nullopt; // nothing to read

    std::string line;
    if (!std::getline(std::cin, line)) return QuitEvent{};
    return parseLine(line);
}

InputEvent ConsoleInputHandler::waitForInput() {
    std::string line;
    if (!std::getline(std::cin, line)) {
        return QuitEvent{};
    }
    return parseLine(line);
}

InputEvent ConsoleInputHandler::parseLine(const std::string& raw) const {
    // Trim and lowercase
    std::string line = raw;
    // ltrim
    line.erase(line.begin(), std::find_if(line.begin(), line.end(),
        [](unsigned char c){ return !std::isspace(c); }));
    // rtrim
    line.erase(std::find_if(line.rbegin(), line.rend(),
        [](unsigned char c){ return !std::isspace(c); }).base(), line.end());

    if (line.empty()) return CommandEvent{"", ""};

    std::string lower = line;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    // ── Quit commands ─────────────────────────────────────────────────────────
    if (lower == "quit" || lower == "exit" || lower == "q") {
        return QuitEvent{};
    }

    // ── Undo / hint / resign / help ───────────────────────────────────────────
    if (lower == "undo" || lower == "u")       return CommandEvent{"undo", ""};
    if (lower == "hint" || lower == "h")       return CommandEvent{"hint", ""};
    if (lower == "resign" || lower == "r")     return CommandEvent{"resign", ""};
    if (lower == "help" || lower == "?")       return CommandEvent{"help", ""};
    if (lower == "flip")                       return CommandEvent{"flip", ""};

    // ── Theme command: "theme classic" / "theme angry" ────────────────────────
    if (lower.rfind("theme", 0) == 0) {
        std::string arg = lower.size() > 5 ? lower.substr(6) : "";
        return CommandEvent{"theme", arg};
    }

    // ── Move parsing ─────────────────────────────────────────────────────────
    // Accepted formats:
    //   "e2e4"   "e2 e4"   "e2-e4"   "e2 to e4"
    //   Promotion: "e7e8q" (appended piece letter)

    // Remove separators
    std::string cleaned;
    for (char c : lower) {
        if (c != ' ' && c != '-') cleaned += c;
    }
    // Remove "to" keyword
    {
        std::size_t pos = cleaned.find("to");
        if (pos != std::string::npos) cleaned.erase(pos, 2);
    }

    // Expect 4 or 5 chars: e2e4 or e2e8q
    if (cleaned.size() >= 4) {
        Position from = Position::fromAlgebraic(cleaned.substr(0, 2));
        Position to   = Position::fromAlgebraic(cleaned.substr(2, 2));

        if (from.isValid() && to.isValid()) {
            Move m = normalMove(from, to);
            // Promotion character?
            if (cleaned.size() >= 5) {
                char promo = cleaned[4];
                // We store the character; ChessRules will map it to a PieceTypeID
                // We encode it as a CommandEvent with the move string so the
                // engine can pass it to the rules. For now, set promotionType = promo char.
                // (The chess plugin will interpret these bytes.)
                m.promotionType = static_cast<PieceTypeID>(promo);
                m.flags |= MoveFlag::Promotion;
            }
            return MoveInputEvent{m};
        }
    }

    // Unknown input — treat as unrecognised command
    return CommandEvent{"unknown", line};
}

} // namespace bge
