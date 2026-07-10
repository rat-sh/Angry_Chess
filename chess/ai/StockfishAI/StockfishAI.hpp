#pragma once
#include "engine/ai/IAIPlayer.hpp"
#include <chrono>
#include <string>
#include <sys/types.h>

namespace angry_chess {

/// AI player that delegates move selection to a local Stockfish process
/// via the Universal Chess Interface (UCI) protocol.
///
/// Stockfish is spawned once at construction and reused across moves.
/// If the executable is not found, construction throws std::runtime_error.
///
/// Extension point: to change search depth or skill level, send additional
/// UCI option commands in launch() (e.g., "setoption name Skill Level value 5").
class StockfishAI final : public bge::IAIPlayer {
public:
    /// \param execPath  Absolute or PATH-relative path to the stockfish binary.
    explicit StockfishAI(std::string execPath = "stockfish");
    ~StockfishAI() override;

    // Non-copyable
    StockfishAI(const StockfishAI&)            = delete;
    StockfishAI& operator=(const StockfishAI&) = delete;

    bge::Move selectMove(const bge::BoardState&   state,
                         const bge::IGameRules&   rules,
                         bge::Color               forPlayer,
                         std::chrono::milliseconds timeLimit) override;

    /// Reset Stockfish's hash tables / move history between games.
    void reset() override;

    [[nodiscard]] std::string_view name() const override { return "stockfish"; }

    /// \return true if the Stockfish process is alive and responsive.
    [[nodiscard]] bool isAlive() const noexcept { return pid_ > 0; }

private:
    void        launch();
    void        sendCommand(std::string_view cmd);
    std::string readLine();
    bge::Move   parseBestMove(std::string_view moveStr,
                              const bge::BoardState& state) const;

    std::string execPath_;
    pid_t       pid_{-1};
    int         fdWrite_{-1}; // our end: write → Stockfish stdin
    int         fdRead_{-1};  // our end: read  ← Stockfish stdout
};

} // namespace angry_chess
