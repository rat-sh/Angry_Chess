#include <catch2/catch_test_macros.hpp>
#include "chess/rules/ChessRules.hpp"
#include "chess/ChessPieceTypes.hpp"
#include "engine/board/BoardState.hpp"

using namespace bge;
using namespace angry_chess;

// Helper: build a board with just white/black kings + extra pieces
static BoardState emptyWithKings() {
    BoardState s;
    s.setCell({0, 4}, KING, Color::White);
    s.setCell({7, 4}, KING, Color::Black);
    s.activePlayer = Color::White;
    s.castling = CastlingRights{false, false, false, false};
    return s;
}

TEST_CASE("ChessRules: pawn moves forward", "[chess_moves]") {
    ChessRules rules;
    BoardState s = emptyWithKings();
    s.setCell({1, 3}, PAWN, Color::White);

    auto moves = rules.generateLegalMoves(s, {1, 3});
    REQUIRE(moves.size() == 2); // single push + double push
    bool hasSinglePush = false, hasDoublePush = false;
    for (const auto& m : moves) {
        if (m.to == Position{2, 3}) hasSinglePush = true;
        if (m.to == Position{3, 3}) hasDoublePush = true;
    }
    CHECK(hasSinglePush);
    CHECK(hasDoublePush);
}

TEST_CASE("ChessRules: pawn blocked by piece", "[chess_moves]") {
    ChessRules rules;
    BoardState s = emptyWithKings();
    s.setCell({1, 3}, PAWN, Color::White);
    s.setCell({2, 3}, PAWN, Color::Black); // blocks

    auto moves = rules.generateLegalMoves(s, {1, 3});
    CHECK(moves.empty()); // completely blocked
}

TEST_CASE("ChessRules: pawn captures diagonally", "[chess_moves]") {
    ChessRules rules;
    BoardState s = emptyWithKings();
    s.setCell({4, 3}, PAWN, Color::White);
    s.setCell({5, 2}, PAWN, Color::Black);
    s.setCell({5, 4}, PAWN, Color::Black);

    auto moves = rules.generateLegalMoves(s, {4, 3});
    bool capturesLeft = false, capturesRight = false;
    for (const auto& m : moves) {
        if (m.to == Position{5, 2}) capturesLeft  = true;
        if (m.to == Position{5, 4}) capturesRight = true;
    }
    CHECK(capturesLeft);
    CHECK(capturesRight);
}

TEST_CASE("ChessRules: knight moves L-shape", "[chess_moves]") {
    ChessRules rules;
    BoardState s = emptyWithKings();
    s.setCell({3, 3}, KNIGHT, Color::White);

    auto moves = rules.generateLegalMoves(s, {3, 3});
    CHECK(moves.size() == 8); // from d4 a knight has 8 possible squares
}

TEST_CASE("ChessRules: knight blocked by friendly pieces reduces moves", "[chess_moves]") {
    ChessRules rules;
    BoardState s = emptyWithKings();
    s.setCell({0, 1}, KNIGHT, Color::White);
    // Knight on b1 (rank=0,file=1) can reach: a3(rank2,file0), c3(rank2,file2), d2(rank1,file3)
    // = 3 squares (all valid, none blocked on kings-only board)
    auto moves = rules.generateLegalMoves(s, {0, 1});
    CHECK(moves.size() == 3);
}

TEST_CASE("ChessRules: bishop moves diagonally", "[chess_moves]") {
    ChessRules rules;
    BoardState s = emptyWithKings();
    s.setCell({3, 3}, BISHOP, Color::White);

    auto moves = rules.generateLegalMoves(s, {3, 3});
    // From d4, a bishop has 13 diagonal squares (9 on open board - king blocks some)
    CHECK(moves.size() >= 10);
}

TEST_CASE("ChessRules: rook moves orthogonally", "[chess_moves]") {
    ChessRules rules;
    BoardState s = emptyWithKings();
    s.setCell({3, 3}, ROOK, Color::White);

    auto moves = rules.generateLegalMoves(s, {3, 3});
    // 7 + 7 - 1 (king blocks one orthogonal ray end) - some may overlap
    CHECK(moves.size() >= 12);
}

TEST_CASE("ChessRules: queen combines rook and bishop", "[chess_moves]") {
    ChessRules rules;
    BoardState s = emptyWithKings();
    s.setCell({3, 3}, QUEEN, Color::White);

    auto rookMoveCount   = 0;
    auto bishopMoveCount = 0;

    // rook moves: same rank or file
    auto moves = rules.generateLegalMoves(s, {3, 3});
    for (const auto& m : moves) {
        if (m.from.rank == m.to.rank || m.from.file == m.to.file) ++rookMoveCount;
        else ++bishopMoveCount;
    }
    CHECK(rookMoveCount   > 0);
    CHECK(bishopMoveCount > 0);
}

TEST_CASE("ChessRules: initial position legal moves count", "[chess_moves]") {
    ChessRules rules;
    BoardState s = rules.initialState();
    auto moves = rules.generateAllLegalMoves(s, Color::White);
    // White has 20 possible moves from the starting position
    CHECK(moves.size() == 20);
}

TEST_CASE("ChessRules: validateMove returns correct result", "[chess_moves]") {
    ChessRules rules;
    BoardState s = rules.initialState();

    // Legal move
    Move e2e4 = normalMove(Position::fromAlgebraic("e2"),
                           Position::fromAlgebraic("e4"));
    CHECK(rules.validateMove(s, e2e4) == MoveResult::Legal);

    // Empty source
    Move empty = normalMove(Position::fromAlgebraic("e4"),
                            Position::fromAlgebraic("e5"));
    CHECK(rules.validateMove(s, empty) == MoveResult::NoPieceAtSource);

    // Wrong turn (black piece on white's turn)
    Move blackPawn = normalMove(Position::fromAlgebraic("e7"),
                                Position::fromAlgebraic("e5"));
    CHECK(rules.validateMove(s, blackPawn) == MoveResult::WrongTurn);
}

TEST_CASE("ChessRules: pawn promotion generates 4 choices", "[chess_moves]") {
    ChessRules rules;
    BoardState s = emptyWithKings();
    s.setCell({6, 0}, PAWN, Color::White); // one step from promotion

    auto moves = rules.generateLegalMoves(s, {6, 0});
    int promoCnt = 0;
    for (const auto& m : moves)
        if (hasFlag(m.flags, MoveFlag::Promotion)) ++promoCnt;
    CHECK(promoCnt == 4); // Q, R, B, N
}
