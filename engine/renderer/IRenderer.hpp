#pragma once
#include "engine/board/BoardState.hpp"
#include "engine/theme/ITheme.hpp"
#include "engine/rules/Move.hpp"
#include <string_view>
#include <vector>

namespace bge {

// ─── Highlight annotation ─────────────────────────────────────────────────────
struct SquareHighlight {
    Position      pos;
    HighlightType type;
};

/// Snapshot of everything the renderer needs for a single frame.
struct RenderContext {
    const BoardState*            state{nullptr};
    const ITheme*                theme{nullptr};
    std::vector<SquareHighlight> highlights;
    std::string                  statusLine;   ///< e.g. "White's turn | Move 14"
    std::string                  messageLine;  ///< Transient message, e.g. "Check!"
    std::optional<Move>          lastMove;
    bool                         flipped{false}; ///< Board displayed from Black's POV
};

/// Stateless renderer interface. Every method is called by GameEngine::render().
class IRenderer {
public:
    virtual ~IRenderer() = default;

    /// Called at the start of a frame; clear buffers / terminal screen.
    virtual void beginFrame() = 0;

    /// Render the board, all pieces, and the highlights provided in ctx.
    virtual void drawBoard(const RenderContext& ctx) = 0;

    /// Render a status bar / message (turn info, game-over text, errors).
    virtual void drawStatus(const RenderContext& ctx) = 0;

    /// Render an input prompt (e.g. "White's move: ").
    virtual void drawPrompt(std::string_view prompt) = 0;

    /// Called at the end of a frame; flush output.
    virtual void endFrame() = 0;

    /// Called on window/terminal resize.
    virtual void onResize() {}

    /// Request promotion type from the user (GUI). Returns kNoPiece by default.
    virtual PieceTypeID showPromotionPicker(Color /*color*/) { return kNoPiece; }
};

} // namespace bge
