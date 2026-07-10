# Angry Chess Engine

A **production-grade, plugin-extensible Board Game Engine** written in modern **C++20**.

## Architecture

```
┌─────────────────────────────────────────────────────┐
│                   Game Plugin Layer                  │
│   chess/  (Pieces · Rules · AI · Theme) │
└───────────────────┬─────────────────────────────────┘
                    │  implements interfaces only
┌───────────────────▼─────────────────────────────────┐
│                  Engine Core Layer                   │
│  GameEngine · EventBus · PluginRegistry             │
├─────────┬──────────┬──────────┬──────────┬──────────┤
│  Board  │  Rules   │  Input   │ Renderer │    AI    │
│ System  │ IGameRules│ Handler │ IRenderer│ IAIPlayer│
└─────────┴──────────┴──────────┴──────────┴──────────┘
```

The **engine** knows nothing about chess. Every game is a **plugin**.

## Project Structure

```
game/
├── engine/               ← Generic engine (no game rules inside)
│   ├── core/             ← GameEngine, EventBus, PluginRegistry, Types
│   ├── board/            ← BoardState (value-semantic, AI-friendly)
│   ├── rules/            ← IGameRules, Move, MoveResult interfaces
│   ├── renderer/         ← IRenderer, ConsoleRenderer, NullRenderer
│   ├── input/            ← IInputHandler, ConsoleInputHandler
│   ├── ai/               ← IAIPlayer, RandomAI
│   └── theme/            ← ITheme
├── chess/                ← Chess plugin (implements engine interfaces)
│   ├── pieces/           ← King, Queen, Rook, Bishop, Knight, Pawn
│   ├── rules/            ← ChessRules, CheckDetector, CastlingRule, ...
│   ├── ai/               ← StockfishAI (optional UCI bridge)
│   └── theme/            ← ClassicTheme, AngryTheme
├── app/
│   └── main.cpp          ← Entry point; wires plugin + engine
└── tests/                ← Catch2 unit tests
```

## Build

### Prerequisites
- CMake ≥ 3.24
- GCC ≥ 12 or Clang ≥ 15 (C++20 support required)
- Internet connection (CMake fetches Catch2 automatically)

### Steps

```bash
cd /home/r/game
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

### Run the game

```bash
./build/angry_chess_game
```

### Run tests

```bash
cd build
ctest --output-on-failure
```

## Game Controls

| Input         | Action                              |
|---------------|-------------------------------------|
| `e2e4`        | Move piece from e2 to e4            |
| `e7e8q`       | Move with queen promotion           |
| `undo`        | Take back the last move             |
| `hint`        | Show a suggested move               |
| `flip`        | Flip the board view                 |
| `resign`      | Resign the current game             |
| `theme angry` | Switch to the Angry emoji theme     |
| `theme classic`| Switch to classic Unicode theme   |
| `help`        | Show all commands                   |
| `quit`        | Exit the game                       |

## Design Principles

| Principle         | How it's applied |
|-------------------|-----------------|
| **SRP**           | Each class has one job: `CheckDetector`, `CastlingRule`, `ChessEvaluator` are separate |
| **OCP**           | Add a new game by writing a plugin — zero engine edits |
| **LSP**           | All implementations satisfy their interface contracts exactly |
| **ISP**           | Thin interfaces: `IGameRules`, `IRenderer`, `ITheme`, `IAIPlayer` |
| **DIP**           | `GameEngine` depends only on interfaces, never on `ChessRules` or `Pawn` |
| **Composition**   | `MinimaxAI` takes an `EvaluatorFn` — inject any evaluator |
| **Value semantics**| `BoardState` is a 72-byte struct — AI clones it freely with no heap cost |
| **Event-driven**  | `EventBus<T>` decouples subsystems (renderer, audio, network) from game logic |

## Adding a New Game (e.g., Checkers)

```cpp
// 1. Create chess/checkers/ (or a new plugin folder) with your rules + pieces + theme
class CheckersRules : public bge::IGameRules { /* ... */ };
class CheckersTheme : public bge::ITheme     { /* ... */ };

// 2. Write the plugin installer
void CheckersPlugin::install(bge::PluginRegistry& registry) {
    registry.registerGame ("checkers", []{ return std::make_unique<CheckersRules>(); });
    registry.registerTheme("checkers", []{ return std::make_unique<CheckersTheme>(); });
}

// 3. In main.cpp, add one line:
CheckersPlugin::install(registry);
// Done — no engine files modified.
```

## License

MIT
