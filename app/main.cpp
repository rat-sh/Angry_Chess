#include "engine/core/GameEngine.hpp"
#include "engine/core/PluginRegistry.hpp"
#include "engine/core/PgnLogger.hpp"
#include "chess/AngryChessPlugin.hpp"
#include "chess/FenParser.hpp"
#include <iostream>
#include <string>
#include <stdexcept>
#include <fstream>

// ─── Banner ───────────────────────────────────────────────────────────────────
static void printBanner() {
    std::cout << "\033[2J\033[H"; // clear screen
    std::cout << "\033[1;38;5;196m"
              << R"(
   ___                              ___  _
  / _ |  ___  ___ _____  __       / __|| |_   ___  ___  ___
 / __ | / _ \/ _ `/ __/ / // // / /__ | ' \ / -_)(_-< (_-<
/_/ |_|/_//_/\_, /_/   /_/\_,_/  \___||_||_|\___|/__//___/
             /___/
)" << "\033[0m"
              << "\033[38;5;214m"
              << "  Production-grade, plugin-extensible Board Game Engine\n"
              << "\033[38;5;240m"
              << "  Engine v1.0  |  Angry Chess plugin  |  C++20\n\n"
              << "\033[0m";
}

// ─── Mode selection ───────────────────────────────────────────────────────────
static bge::GameConfig askConfig() {
    bge::GameConfig cfg;
    cfg.gameName  = "angry_chess";
    cfg.themeName = "classic";

    std::cout << "\033[1mSelect mode:\033[0m\n"
              << "  1) Human vs Human\n"
              << "  2) Human vs Stockfish  (you play White)\n"
              << "  3) Stockfish vs Human  (you play Black)\n"
              << "  4) AI vs AI  (watch)\n"
              << "\nChoice [1-4, default 2]: ";

    int choice = 2;
    std::string line;
    if (std::getline(std::cin, line) && !line.empty()) {
        try { choice = std::stoi(line); } catch (...) {}
    }

    switch (choice) {
        case 1:
            cfg.whiteIsAI = false;
            cfg.blackIsAI = false;
            break;
        default:
        case 2:
            cfg.whiteIsAI   = false;
            cfg.blackIsAI   = true;
            cfg.blackAIName = "stockfish";
            break;
        case 3:
            cfg.whiteIsAI   = true;
            cfg.blackIsAI   = false;
            cfg.whiteAIName = "stockfish";
            break;
        case 4:
            cfg.whiteIsAI   = true;
            cfg.blackIsAI   = true;
            cfg.whiteAIName = "random";
            cfg.blackAIName = "random";
            cfg.aiMoveDelay = std::chrono::milliseconds{600};
            break;
    }
    return cfg;
}

// ─── CLI argument parsing ─────────────────────────────────────────────────────
struct CliArgs {
    std::string fenString;   // optional custom starting position
    std::string pgnOutput;   // optional PGN output file
    bool        showHelp{false};
};

static CliArgs parseArgs(int argc, char** argv) {
    CliArgs args;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            args.showHelp = true;
        } else if ((arg == "--fen" || arg == "-f") && i + 1 < argc) {
            args.fenString = argv[++i];
        } else if ((arg == "--pgn" || arg == "-p") && i + 1 < argc) {
            args.pgnOutput = argv[++i];
        }
    }
    return args;
}

static void printHelp(const char* prog) {
    std::cout << "Usage: " << prog << " [options]\n\n"
              << "Options:\n"
              << "  --fen <fen>   Start from a custom FEN position\n"
              << "  --pgn <file>  Write game PGN to file after game over\n"
              << "  --help        Show this help\n\n"
              << "Game commands:\n"
              << "  e2e4       Move piece (source + destination)\n"
              << "  e7e8q      Move with promotion (q/r/b/n)\n"
              << "  undo       Take back last move\n"
              << "  hint       Show a suggested move\n"
              << "  flip       Flip the board\n"
              << "  resign     Resign the game\n"
              << "  help       Show in-game help\n"
              << "  quit       Exit\n";
}

// ─── main ─────────────────────────────────────────────────────────────────────
int main(int argc, char** argv) {
    try {
        CliArgs cli = parseArgs(argc, argv);
        if (cli.showHelp) {
            printHelp(argv[0]);
            return 0;
        }

        printBanner();

        // ── 1. Plugin registry ────────────────────────────────────────────────
        bge::PluginRegistry registry;
        angry_chess::AngryChessPlugin::install(registry);

        // ── 2. Game mode ──────────────────────────────────────────────────────
        bge::GameConfig cfg = askConfig();

        // ── 3. Assemble subsystems ────────────────────────────────────────────
        auto rules    = registry.createGame        (cfg.gameName);
        auto renderer = registry.createRenderer    ("console");
        auto input    = registry.createInputHandler("console");
        auto theme    = registry.createTheme       (cfg.themeName);

        bge::GameEngine engine(
            std::move(rules),
            std::move(renderer),
            std::move(input),
            std::move(theme),
            cfg
        );

        // ── 4. Custom FEN starting position ───────────────────────────────────
        if (!cli.fenString.empty()) {
            std::cout << "\033[38;5;226mLoading FEN: \033[0m" << cli.fenString << "\n\n";
            bge::BoardState custom = bge::FenParser::parse(cli.fenString);
            engine.boardState() = custom;
        }

        // ── 5. Wire AI ────────────────────────────────────────────────────────
        if (cfg.whiteIsAI)
            engine.setWhiteAI(registry.createAI(cfg.whiteAIName));
        if (cfg.blackIsAI)
            engine.setBlackAI(registry.createAI(cfg.blackAIName));

        // ── 6. PGN logger ─────────────────────────────────────────────────────
        bge::PgnLogger pgn("White", "Black");
        engine.onMoveMade().subscribe([&](const bge::MoveMadeEvent& ev) {
            (void)ev; // move already recorded through onMoveMade
        });

        // ── 7. Game-over handler ──────────────────────────────────────────────
        engine.onGameOver().subscribe([&](const bge::GameOverEvent& ev) {
            std::cout << "\n\033[1;38;5;226m";
            if (ev.winnerColor > 0) {
                std::cout << "  ♔ White wins!\n";
                pgn.setResult("1-0");
            } else if (ev.winnerColor < 0) {
                std::cout << "  ♚ Black wins!\n";
                pgn.setResult("0-1");
            } else {
                std::cout << "  ½-½ Draw!\n";
                pgn.setResult("1/2-1/2");
            }
            std::cout << "\033[0m\n";

            // Write PGN file if requested
            if (!cli.pgnOutput.empty()) {
                std::ofstream f(cli.pgnOutput);
                if (f) {
                    f << pgn.toPgn();
                    std::cout << "\033[38;5;240mPGN saved to: "
                              << cli.pgnOutput << "\033[0m\n";
                }
            }
        });

        // ── 8. Run ────────────────────────────────────────────────────────────
        engine.run();

        std::cout << "\033[38;5;240mThanks for playing Angry Chess!\033[0m\n\n";
        return 0;

    } catch (const std::exception& ex) {
        std::cerr << "\n\033[1;31m[FATAL]\033[0m " << ex.what() << "\n";
        return 1;
    }
}
