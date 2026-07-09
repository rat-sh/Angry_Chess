#pragma once
#include <cstdint>

namespace bge {

/// Board square shading (for rendering).
enum class SquareShade : uint8_t {
    Light,
    Dark,
};

/// Returns the shade of a square given its rank and file.
inline SquareShade squareShade(int8_t rank, int8_t file) noexcept {
    return ((rank + file) % 2 == 0) ? SquareShade::Light : SquareShade::Dark;
}

} // namespace bge
