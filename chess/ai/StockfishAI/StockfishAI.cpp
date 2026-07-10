/// StockfishAI.cpp — UCI bridge between the Chess Framework and a local Stockfish binary.
///
/// Protocol flow (per move):
///   Constructor : spawn process, send "uci", wait for "uciok"
///   selectMove  : send "position fen <FEN>", send "go movetime <ms>", read "bestmove <move>"
///   Destructor  : send "quit", close pipes, wait for process exit
///
/// The Stockfish process is kept alive between moves to amortise startup cost
/// and to preserve hash table state across the game.

#include "chess/ai/StockfishAI/StockfishAI.hpp"
#include "chess/FenParser.hpp"
#include "engine/rules/Move.hpp"
#include "engine/core/Types.hpp"

#include <stdexcept>
#include <string>
#include <cstring>
#include <iostream>

// POSIX process / pipe API
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

namespace angry_chess {

using namespace bge;

// ─── Construction / Destruction ───────────────────────────────────────────────

StockfishAI::StockfishAI(std::string execPath)
    : execPath_(std::move(execPath))
{
    launch();
}

StockfishAI::~StockfishAI() {
    if (pid_ > 0) {
        try { sendCommand("quit"); } catch (...) {}
        // Give Stockfish 500 ms to exit cleanly, then kill it
        ::usleep(500'000);
        ::kill(pid_, SIGTERM);
        ::waitpid(pid_, nullptr, WNOHANG);
    }
    if (fdWrite_ >= 0) ::close(fdWrite_);
    if (fdRead_  >= 0) ::close(fdRead_);
}

// ─── Launch ───────────────────────────────────────────────────────────────────

void StockfishAI::launch() {
    // Create two pipes:  parent→child (stdin), child→parent (stdout)
    int pipeIn[2];   // [0]=read end (child stdin),  [1]=write end (parent writes)
    int pipeOut[2];  // [0]=read end (parent reads), [1]=write end (child stdout)

    if (::pipe(pipeIn)  != 0 ||
        ::pipe(pipeOut) != 0) {
        throw std::runtime_error("StockfishAI: pipe() failed");
    }

    pid_ = ::fork();
    if (pid_ < 0) {
        throw std::runtime_error("StockfishAI: fork() failed");
    }

    if (pid_ == 0) {
        // ── Child process: become Stockfish ───────────────────────────────────
        ::dup2(pipeIn[0],  STDIN_FILENO);
        ::dup2(pipeOut[1], STDOUT_FILENO);
        ::dup2(pipeOut[1], STDERR_FILENO); // suppress stderr noise

        ::close(pipeIn[0]);  ::close(pipeIn[1]);
        ::close(pipeOut[0]); ::close(pipeOut[1]);

        ::execlp(execPath_.c_str(), execPath_.c_str(), nullptr);
        // If execlp returns, the binary was not found
        std::cerr << "[StockfishAI] exec failed: " << execPath_ << "\n";
        ::_exit(1);
    }

    // ── Parent process ────────────────────────────────────────────────────────
    ::close(pipeIn[0]);
    ::close(pipeOut[1]);

    fdWrite_ = pipeIn[1];
    fdRead_  = pipeOut[0];

    // Make read end non-blocking so readLine() can use select() with a timeout
    ::fcntl(fdRead_, F_SETFL, O_NONBLOCK);

    // UCI handshake: send "uci", wait for "uciok"
    sendCommand("uci");
    for (int tries = 0; tries < 100; ++tries) {
        std::string line = readLine();
        if (line.rfind("uciok", 0) == 0) break;
        if (tries == 99) {
            throw std::runtime_error(
                "StockfishAI: Stockfish did not respond with 'uciok'. "
                "Is '" + execPath_ + "' installed? (apt install stockfish)");
        }
    }

    // Set to a moderate skill level (0–20). Adjust here if needed.
    sendCommand("setoption name Skill Level value 10");
    sendCommand("isready");
    for (int tries = 0; tries < 100; ++tries) {
        if (readLine().rfind("readyok", 0) == 0) break;
    }
}

// ─── reset ────────────────────────────────────────────────────────────────────

void StockfishAI::reset() {
    sendCommand("ucinewgame");
    sendCommand("isready");
    for (int tries = 0; tries < 50; ++tries) {
        if (readLine().rfind("readyok", 0) == 0) break;
    }
}

// ─── selectMove ──────────────────────────────────────────────────────────────

Move StockfishAI::selectMove(const BoardState&    state,
                              const IGameRules&    /*rules*/,
                              Color                /*forPlayer*/,
                              std::chrono::milliseconds timeLimit)
{
    // Convert current position to FEN
    std::string fen = bge::FenParser::generate(state);

    sendCommand("position fen " + fen);
    sendCommand("go movetime " + std::to_string(timeLimit.count()));

    // Read lines until we get "bestmove <move>"
    for (int tries = 0; tries < 2000; ++tries) {
        std::string line = readLine();
        if (line.rfind("bestmove", 0) == 0) {
            // "bestmove e2e4" or "bestmove e2e4 ponder e7e5"
            // Extract the move token after "bestmove "
            std::string moveStr = line.size() > 9 ? line.substr(9) : "";
            // Trim to first whitespace
            auto sp = moveStr.find(' ');
            if (sp != std::string::npos) moveStr = moveStr.substr(0, sp);

            if (moveStr == "(none)" || moveStr.empty()) return {}; // no legal move
            return parseBestMove(moveStr, state);
        }
        ::usleep(5000); // 5 ms wait between polls
    }

    // Timeout — return empty (invalid) move; engine will treat as game over
    return {};
}

// ─── parseBestMove ────────────────────────────────────────────────────────────

Move StockfishAI::parseBestMove(std::string_view moveStr,
                                const BoardState& /*state*/) const
{
    // UCI format: e2e4  (4 chars) or e7e8q (5 chars with promotion)
    if (moveStr.size() < 4) return {};

    Position from = Position::fromAlgebraic(std::string(moveStr.substr(0, 2)));
    Position to   = Position::fromAlgebraic(std::string(moveStr.substr(2, 2)));

    if (!from.isValid() || !to.isValid()) return {};

    Move m = normalMove(from, to);

    // Promotion character (5th char)
    if (moveStr.size() >= 5) {
        char promo = moveStr[4];
        m.flags |= MoveFlag::Promotion;
        m.promotionType = static_cast<PieceTypeID>(promo); // ChessRules::applyMove resolves this
    }

    return m;
}

// ─── I/O helpers ─────────────────────────────────────────────────────────────

void StockfishAI::sendCommand(std::string_view cmd) {
    std::string msg{cmd};
    msg += '\n';
    if (::write(fdWrite_, msg.data(), msg.size()) < 0) {
        // Stockfish died — mark as dead
        ::close(fdWrite_); fdWrite_ = -1;
        pid_ = -1;
    }
}

std::string StockfishAI::readLine() {
    // Read one line from Stockfish, with a per-character retry using select().
    // Returns empty string on timeout or error.
    std::string line;
    line.reserve(128);

    for (int attempts = 0; attempts < 5000; ++attempts) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fdRead_, &fds);
        struct timeval tv{0, 10'000}; // 10 ms per attempt

        int ready = ::select(fdRead_ + 1, &fds, nullptr, nullptr, &tv);
        if (ready <= 0) continue;

        char c;
        ssize_t n = ::read(fdRead_, &c, 1);
        if (n <= 0) break;
        if (c == '\n') break;
        if (c != '\r') line += c;
    }
    return line;
}

} // namespace angry_chess
