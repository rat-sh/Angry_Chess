#pragma once
#include "engine/core/Types.hpp"
#include "engine/board/BoardState.hpp"
#include <vector>

namespace bge {

/// A typed collection of all pieces belonging to one color on the board.
/// Derived from a BoardState snapshot on demand.
struct PieceOnBoard {
    Position    pos;
    PieceTypeID typeId;
    Color       color;
};

/// Utility: extract all pieces of `color` from `state`.
[[nodiscard]] inline std::vector<PieceOnBoard> getPieceSet(
    const BoardState& state, Color color) {
    std::vector<PieceOnBoard> pieces;
    for (int8_t r = 0; r < 8; ++r) {
        for (int8_t f = 0; f < 8; ++f) {
            Position pos{r, f};
            if (state.colorAt(pos) == color) {
                pieces.push_back({pos, state.typeAt(pos), color});
            }
        }
    }
    return pieces;
}

} // namespace bge
