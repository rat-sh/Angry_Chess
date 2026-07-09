#pragma once
#include "engine/core/Types.hpp"
#include <string>
#include <string_view>

namespace bge {

/// Lightweight descriptor for a piece type registered by a game plugin.
/// This is pure metadata — behaviour (move generation) lives in the plugin's
/// move-generator functions, not here.
struct PieceDescriptor {
    PieceTypeID      id{kNoPiece};
    std::string      name;         ///< e.g. "Pawn", "Knight"
    char             ascii{' '};   ///< Fallback single-char display
    int              materialValue{0}; ///< Default material value (centipawns)
};

/// Interface for a concrete piece (used when pieces need polymorphic behaviour).
/// Most games use the data-driven PieceDescriptor + move-generator function approach
/// instead of this interface.
class IPiece {
public:
    virtual ~IPiece() = default;
    [[nodiscard]] virtual PieceTypeID    typeId()   const = 0;
    [[nodiscard]] virtual std::string_view name()   const = 0;
    [[nodiscard]] virtual Color          color()    const = 0;
};

} // namespace bge
