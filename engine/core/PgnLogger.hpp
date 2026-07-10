#pragma once
// PgnLogger has moved to chess/notation/Pgn/PgnLogger.hpp
// This header is kept for backwards compatibility with existing includes.
#include "chess/notation/Pgn/PgnLogger.hpp"

// Legacy alias in bge namespace for code that uses bge::PgnLogger
namespace bge {
    using PgnLogger = angry_chess::PgnLogger;
} // namespace bge
