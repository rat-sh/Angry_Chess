<div align="center">

<!-- TODO: Replace with actual banner image -->
<img src="https://via.placeholder.com/900x200/16161e/FFD700?text=♟+Angry+Chess" alt="Angry Chess Banner" width="100%"/>

# ♟ Angry Chess

**A modular, cross-platform chess game written in C++20.**  
Play against Stockfish, challenge a friend, or watch two AIs battle it out — all from your terminal or a graphical window.

<br/>

[![Build](https://img.shields.io/badge/build-passing-brightgreen?style=flat-square&logo=github-actions)](https://github.com/rat-sh/Angry_Chess)
[![C++20](https://img.shields.io/badge/C++-20-00599C?style=flat-square&logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp/20)
[![CMake](https://img.shields.io/badge/CMake-3.24+-064F8C?style=flat-square&logo=cmake&logoColor=white)](https://cmake.org/)
[![SDL2](https://img.shields.io/badge/SDL-2-1C4A6E?style=flat-square)](https://www.libsdl.org/)
[![License](https://img.shields.io/badge/License-MIT-A855F7?style=flat-square)](LICENSE)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-FF6B6B?style=flat-square)](CONTRIBUTING.md)
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-4ADE80?style=flat-square)](#installation)

<br/>

[**Play Now**](#quick-start) · [**Report a Bug**](https://github.com/rat-sh/Angry_Chess/issues) · [**Request a Feature**](https://github.com/rat-sh/Angry_Chess/issues) · [**Contribute**](#contributing)

</div>

---

## What is Angry Chess?

Angry Chess is an open-source chess game built on a **clean, plugin-extensible C++20 engine**. It ships with a full chess ruleset, a UCI-based Stockfish AI bridge, two visual themes, PGN export, and both a graphical SDL2 client and a terminal interface — all wired together through a lightweight event bus and plugin registry.

It is designed to be readable, hackable, and easy to extend. If you want to add a new AI, a new theme, or even a different board game on top of the same engine — the architecture makes that straightforward.

> **Current Status:** Early Development / Alpha — expect rough edges. Contributions welcome.

---

## Features

| Feature | Console | SDL2 |
|---|:---:|:---:|
| Human vs Human | ✅ | ✅ |
| Human vs Stockfish AI | ✅ | ✅ |
| Stockfish vs Human | ✅ | ✅ |
| AI vs AI Spectator Mode | ✅ | ✅ |
| Undo Last Move | ✅ | ✅ |
| Board Flip | ✅ | ✅ |
| Resign | ✅ | ✅ |
| Move Hint | ✅ | — |
| Theme Switching (Classic / Angry) | ✅ | ✅ |
| Custom FEN Starting Position | ✅ | ✅ |
| PGN Export | ✅ | — |
| Keyboard Navigation (Esc to quit) | — | ✅ |
| Graphical Board (SDL2 + TTF) | — | ✅ |
| Taunt Messages on Check / Checkmate | ✅ | ✅ |
| Stockfish Graceful Fallback to Random AI | ✅ | ✅ |

---

## Screenshots

> **TODO:** Screenshots and recordings coming soon. Run the project and share yours!

<!-- TODO: Add actual screenshots below -->
<!-- ![Console Mode](.github/screenshots/console.png) -->
<!-- ![SDL2 Mode](.github/screenshots/sdl.png) -->

---

## Quick Start

```bash
# 1. Clone
git clone https://github.com/rat-sh/Angry_Chess.git
cd Angry_Chess

# 2. Build (Release)
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel

# 3a. Run the graphical version (requires SDL2)
./build/angry_chess_sdl

# 3b. Run the terminal version
./build/angry_chess_game
```

That's it. If Stockfish is installed and on your `PATH`, the AI works automatically. If not, the game falls back to a built-in random AI and prints a warning — no crash.

---

## Installation

### Prerequisites

| Dependency | Required For | Install |
|---|---|---|
| C++20 compiler (GCC 10+, Clang 10+) | All targets | System package manager |
| CMake 3.24+ | All targets | `apt install cmake` |
| SDL2 + SDL2_ttf | Graphical client | See below |
| Stockfish binary | AI opponent | Optional — falls back to Random AI |

### Linux (Ubuntu / Debian)

```bash
# Build tools
sudo apt install build-essential cmake

# Graphical client dependencies (optional but recommended)
sudo apt install libsdl2-dev libsdl2-ttf-dev

# Stockfish AI (optional)
sudo apt install stockfish
```

### Windows

Install [CMake](https://cmake.org/download/) and a C++20-capable compiler (MSVC via Visual Studio, or [MSYS2/MinGW](https://www.msys2.org/)).

Download SDL2 and SDL2_ttf development libraries from [libsdl.org](https://www.libsdl.org/download-2.0.php) and extract them to a known path. Download the [Stockfish binary](https://stockfishchess.org/download/) and add it to your system `PATH`.

```bat
cmake -B build -DCMAKE_BUILD_TYPE=Release -DSDL2_DIR=C:\path\to\SDL2
cmake --build build --config Release
```

### Build (all platforms)

```bash
# Debug build with AddressSanitizer and UBSan
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel

# Run unit tests
cd build && ctest --output-on-failure
```

---

## Controls

### SDL2 Graphical Client

| Key / Action | Description |
|---|---|
| **Click** | Select piece / confirm move |
| **T** | Cycle to the next theme |
| **R** | Restart (returns to mode menu) |
| **U** | Undo last move |
| **F** | Flip board perspective |
| **Esc** | Quit the application |

### Console / Terminal Client

| Command | Description |
|---|---|
| `e2e4` | Move a piece (source + destination in UCI format) |
| `e7e8q` | Move with pawn promotion (`q` / `r` / `b` / `n`) |
| `undo` | Take back the last move |
| `hint` | Show a suggested move |
| `flip` | Flip the board |
| `resign` | Resign the current game |
| `theme classic` / `theme angry` | Switch active theme |
| `quit` | Exit the game |

#### CLI flags (console client)

```bash
./angry_chess_game --fen "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
./angry_chess_game --pgn output.pgn
```

---

## Architecture

Angry Chess separates concerns across three layers, keeping the engine reusable and the chess logic self-contained.

```
┌──────────────────────────────────────────────┐
│              App Layer (app/)                │
│   main.cpp (console)  │  sdl_main.cpp (GUI)  │
└────────────────┬─────────────────────────────┘
                 │ uses
┌────────────────▼─────────────────────────────┐
│           Engine Layer (engine/)             │
│  GameEngine · PluginRegistry · EventBus      │
│  IRenderer · IInputHandler · ITheme · IAI    │
└────────────────┬─────────────────────────────┘
                 │ plugin-registered by
┌────────────────▼─────────────────────────────┐
│           Chess Layer (chess/)               │
│  ChessRules · StockfishAI (UCI) · PgnLogger  │
│  AngryTheme · ClassicTheme · FenParser       │
└──────────────────────────────────────────────┘
```

- **Engine Layer** (`engine/`) — generic and game-agnostic. Owns the game loop, event bus, plugin registry, rendering and input abstractions, and theme system. It has no knowledge of chess-specific rules.
- **Chess Layer** (`chess/`) — the Angry Chess plugin. Implements chess rules, Stockfish UCI bridging, FEN parsing, PGN logging, and both visual themes. Registered into the engine at startup via `AngryChessPlugin::install()`.
- **App Layer** (`app/`) — thin entry points that wire SDL2 or console I/O into the engine and launch the game loop.

This means you could replace the chess plugin with a different game (checkers, custom rules, etc.) without touching the engine.

---

## Project Structure

```text
Angry_Chess/
├── app/
│   ├── main.cpp              # Console entry point
│   └── sdl_main.cpp          # SDL2 graphical entry point
├── chess/
│   ├── AngryChessPlugin.cpp  # Registers all chess components
│   ├── ai/
│   │   └── StockfishAI/      # UCI bridge to Stockfish process
│   ├── notation/
│   │   ├── Pgn/              # PGN logger
│   │   └── Fen/              # FEN parser
│   ├── pieces/               # Chess piece type definitions
│   ├── rules/                # Full chess ruleset & move generation
│   └── theme/
│       ├── ClassicTheme.hpp  # Standard board colours
│       └── AngryTheme.hpp    # Fiery orange-red theme with taunts
├── engine/
│   ├── core/                 # GameEngine, PluginRegistry, EventBus
│   ├── ai/                   # RandomAI (built-in fallback)
│   ├── board/                # BoardState
│   ├── input/                # ConsoleInputHandler, SDL2InputHandler
│   ├── renderer/             # ConsoleRenderer, SDL2Renderer
│   ├── rules/                # IGame interface
│   └── theme/                # ITheme interface
├── tests/                    # Catch2 unit tests
└── CMakeLists.txt
```

---

## Roadmap

Items below are **planned** and do not yet exist in the codebase.

- [ ] **Online multiplayer** — play over a network
- [ ] **Spectator mode for online games**
- [ ] **Tournament bracket mode**
- [ ] **Mod / custom ruleset support** (new piece types, board sizes)
- [ ] **macOS support**
- [ ] **Steam release**
- [ ] **Configurable Stockfish search depth / time**
- [ ] **Opening book display**

Have an idea? [Open an issue](https://github.com/rat-sh/Angry_Chess/issues) or start a discussion.

---

## Contributing

Angry Chess is actively looking for contributors. Whether you want to fix a bug, add a feature, improve the docs, or write tests — all contributions are welcome.

### Getting Started

```bash
# 1. Fork the repo on GitHub, then clone your fork
git clone https://github.com/<your-username>/Angry_Chess.git
cd Angry_Chess

# 2. Create a feature branch
git checkout -b feature/your-feature-name

# 3. Build in Debug mode (sanitizers catch memory bugs early)
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel

# 4. Run the tests
cd build && ctest --output-on-failure && cd ..

# 5. Commit your changes
git add .
git commit -m "feat: describe what you changed"

# 6. Push and open a Pull Request
git push origin feature/your-feature-name
```

Then open a Pull Request on GitHub. A maintainer will review it.

### Good First Issues

Look for issues tagged [`good first issue`](https://github.com/rat-sh/Angry_Chess/issues?q=label%3A%22good+first+issue%22) on the tracker. Writing a new theme, adding a Catch2 test, or improving the console rendering are all great starting points.

---

## Code Style

- **Standard:** C++20, no extensions (`CMAKE_CXX_EXTENSIONS OFF`).
- **Ownership:** Prefer `std::unique_ptr` and RAII. Avoid raw `new`/`delete`.
- **Layer boundary:** `engine/` must remain game-agnostic. Chess-specific code belongs in `chess/`. Never import chess headers from engine code.
- **Warnings:** Code must compile cleanly with `-Wall -Wextra -Wpedantic`.
- **Tests:** Add a Catch2 test for any non-trivial logic you introduce.

---

## Documentation

> **TODO:** Detailed documentation is a work in progress.

| Document | Status |
|---|---|
| Architecture Guide | TODO |
| Chess Rules Implementation | TODO |
| Build Guide | This README |
| Contributing Guide | This README |
| Roadmap | [See above](#roadmap) |
| License | [MIT](LICENSE) |

---

## License

Distributed under the [MIT License](LICENSE). You are free to use, modify, and distribute this software. See the `LICENSE` file for the full text.

---

## Acknowledgements

- [**Stockfish**](https://stockfishchess.org/) — The open-source chess engine powering the AI. Angry Chess uses Stockfish as an external process over the UCI protocol.
- [**SDL**](https://www.libsdl.org/) — Cross-platform multimedia library used for the graphical client.
- [**Catch2**](https://github.com/catchorg/Catch2) — The C++ test framework used for unit tests.

---

## FAQ

**Q: Stockfish isn't working — the AI makes random moves.**  
A: Ensure Stockfish is installed and on your `PATH`. On Linux: `sudo apt install stockfish`. On Windows: download from [stockfishchess.org](https://stockfishchess.org/download/) and add to `PATH`. When Stockfish is not found, the game prints a yellow warning and falls back to the built-in random AI instead of crashing.

**Q: SDL2 target isn't being built.**  
A: Run `sudo apt install libsdl2-dev libsdl2-ttf-dev` (Linux). CMake will detect the libraries automatically. If they're installed in a non-standard path, pass `-DSDL2_DIR=...` to CMake.

**Q: Can I run this on macOS?**  
A: It may work with minor adjustments (the Stockfish bridge uses POSIX `fork`/`pipe`), but macOS is not an officially tested or supported platform yet.

**Q: Can I use the engine for a different game?**  
A: Yes. The `engine/` layer is intentionally game-agnostic. Implement the `IGame`, `ITheme`, and optionally `IAIPlayer` interfaces, register them via `PluginRegistry`, and the engine will run your game.

**Q: Where do I report bugs?**  
A: Please open an issue on [GitHub Issues](https://github.com/rat-sh/Angry_Chess/issues) with a description of the problem, your OS, compiler version, and steps to reproduce.

---

## Support

- **Bug reports:** [GitHub Issues](https://github.com/rat-sh/Angry_Chess/issues)
- **Feature requests:** [GitHub Issues](https://github.com/rat-sh/Angry_Chess/issues)
- **Discussions:** [GitHub Discussions](https://github.com/rat-sh/Angry_Chess/discussions)

If Angry Chess has been useful to you, consider leaving a ⭐ on GitHub — it helps others find the project.

---

<!-- TODO: Add a real star history chart from https://star-history.com -->
<div align="center">

**[⭐ Star History]** — coming soon.

<br/>

*Angry Chess is an independent open-source project, not affiliated with FIDE, Stockfish, or SDL.*

</div>
