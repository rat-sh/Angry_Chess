#include "engine/input/SDL2InputHandler.hpp"
#include "engine/rules/Move.hpp"
#include "games/angry_chess/ChessPieceTypes.hpp"

namespace bge {

Position SDL2InputHandler::pixelToPosition(int x, int y) const {
    int col = x / SDL2Renderer::kSqSize;
    int row = y / SDL2Renderer::kSqSize;
    int8_t rank = flipped_ ? static_cast<int8_t>(row)
                           : static_cast<int8_t>(7 - row);
    int8_t file = flipped_ ? static_cast<int8_t>(7 - col)
                           : static_cast<int8_t>(col);
    return {rank, file};
}

bool SDL2InputHandler::isOnBoard(int x, int y) const {
    return x >= 0 && x < SDL2Renderer::kBoardW
        && y >= 0 && y < SDL2Renderer::kBoardH;
}

InputEvent SDL2InputHandler::waitForInput() {
    SDL_Event ev;
    while (true) {
        if (!SDL_WaitEvent(&ev)) continue;

        if (ev.type == SDL_QUIT)
            return QuitEvent{};

        if (ev.type == SDL_KEYDOWN) {
            switch (ev.key.keysym.sym) {
                case SDLK_ESCAPE: return QuitEvent{};
                case SDLK_t:
                    renderer_.nextTheme();
                    return CommandEvent{"theme", ""};
                case SDLK_r:  return CommandEvent{"restart", ""};
                case SDLK_u:  return CommandEvent{"undo",    ""};
                case SDLK_f:
                    flipped_ = !flipped_;
                    return CommandEvent{"flip", ""};
                case SDLK_h:  return CommandEvent{"hint",    ""};
                default: break;
            }
        }

        if (ev.type == SDL_MOUSEBUTTONDOWN) {
            int mx = ev.button.x, my = ev.button.y;

            if (!isOnBoard(mx, my)) {
                // Click outside board = deselect
                if (pendingFrom_.has_value()) {
                    pendingFrom_.reset();
                    return CommandEvent{"deselect", ""};
                }
                continue;
            }

            Position clicked = pixelToPosition(mx, my);

            if (ev.button.button == SDL_BUTTON_RIGHT) {
                // Right click = deselect
                pendingFrom_.reset();
                return CommandEvent{"deselect", ""};
            }

            if (!pendingFrom_.has_value()) {
                // First click: select piece
                pendingFrom_ = clicked;
                // Use a special "select" command so engine highlights legal moves
                std::string arg = std::to_string(clicked.rank) + " "
                                + std::to_string(clicked.file);
                return CommandEvent{"select", arg};
            } else {
                Position from = *pendingFrom_;
                pendingFrom_.reset();

                if (from == clicked) {
                    // Clicked same square = deselect
                    return CommandEvent{"deselect", ""};
                }

                // Second click: emit a move
                Move m = normalMove(from, clicked);
                return MoveInputEvent{m};
            }
        }
    }
}

std::optional<InputEvent> SDL2InputHandler::poll() {
    SDL_Event ev;
    if (!SDL_PollEvent(&ev)) return std::nullopt;

    if (ev.type == SDL_QUIT) return QuitEvent{};
    if (ev.type == SDL_KEYDOWN) {
        switch (ev.key.keysym.sym) {
            case SDLK_ESCAPE: return QuitEvent{};
            case SDLK_t:
                renderer_.nextTheme();
                return CommandEvent{"theme", ""};
            case SDLK_r:  return CommandEvent{"restart", ""};
            case SDLK_u:  return CommandEvent{"undo",    ""};
            case SDLK_f:
                flipped_ = !flipped_;
                return CommandEvent{"flip", ""};
            case SDLK_h:  return CommandEvent{"hint",    ""};
            default: break;
        }
    }
    // Note: Mouse events are complex to poll because of the state machine (pendingFrom_).
    // For now, poll() only handles keyboard shortcuts since it's mostly used by AI polling.
    return std::nullopt;
}

} // namespace bge
