#pragma once
#include "engine/board/BoardState.hpp"

namespace bge {

/// Optional stateless board-query interface.
/// BoardState covers most use-cases. Implement IBoard if you need
/// additional game-specific queries (e.g., board topology for hex grids).
class IBoard {
public:
    virtual ~IBoard() = default;

    /// Number of ranks (rows).
    [[nodiscard]] virtual int ranks() const = 0;

    /// Number of files (columns).
    [[nodiscard]] virtual int files() const = 0;

    /// Returns the current snapshot.
    [[nodiscard]] virtual const BoardState& snapshot() const = 0;
};

} // namespace bge
