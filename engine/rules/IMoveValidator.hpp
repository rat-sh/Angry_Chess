#pragma once
#include "engine/board/BoardState.hpp"
#include "engine/rules/Move.hpp"
#include "engine/rules/MoveResult.hpp"

namespace bge {

/// Single-responsibility interface for move validation.
/// Implementations can be composed (e.g. chain of responsibility).
class IMoveValidator {
public:
    virtual ~IMoveValidator() = default;

    /// Returns Legal if the move passes this validator's check; otherwise an error code.
    [[nodiscard]] virtual MoveResult validate(
        const BoardState& state,
        const Move&       move) const = 0;
};

} // namespace bge
