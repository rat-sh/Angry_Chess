#pragma once
#include <cstdint>

namespace bge {

enum class GameStateID : uint8_t {
    Menu,
    Playing,
    Paused,
    GameOver,
};

} // namespace bge
