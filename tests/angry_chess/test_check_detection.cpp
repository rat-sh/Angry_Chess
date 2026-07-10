#include <catch2/catch_test_macros.hpp>
#include "chess/rules/ChessRules.hpp"
#include "chess/rules/CheckDetector.hpp"
#include "chess/ChessPieceTypes.hpp"

using namespace bge;
using namespace angry_chess;

static BoardState kingsOnly() {
    BoardState s;
    s.setCell({0, 4}, KING, Color::White);
    s.setCell({7, 4}, KING, Color::Black);
    s.activePlayer = Color::White;
    s.castling = CastlingRights{false, false, false, false};
    return s;
}

TEST_CASE("CheckDetector: king not in check on empty board", "[check]") {
    BoardState s = kingsOnly();
    CHECK_FALSE(isInCheck(s, Color::White));
    CHECK_FALSE(isInCheck(s, Color::Black));
}

TEST_CASE("CheckDetector: rook gives check", "[check]") {
    BoardState s = kingsOnly();
    s.setCell({0, 0}, ROOK, Color::Black); // black rook on rank 0, attacks white king
    CHECK(isInCheck(s, Color::White));
    CHECK_FALSE(isInCheck(s, Color::Black));
}

TEST_CASE("CheckDetector: bishop gives check", "[check]") {
    BoardState s = kingsOnly();
    // Black bishop on b2 (rank 1, file 1) — diagonal to white king on e1 (rank 0, file 4)?
    // Actually bishop on a5 (rank 4, file 0) attacks e1 (rank 0, file 4): diff = 4,4 diagonal
    s.setCell({4, 0}, BISHOP, Color::Black);
    CHECK(isInCheck(s, Color::White));
}

TEST_CASE("CheckDetector: piece blocking check", "[check]") {
    BoardState s = kingsOnly();
    s.setCell({0, 0}, ROOK, Color::Black); // would attack white king
    s.setCell({0, 2}, KNIGHT, Color::White); // blocks the attack
    CHECK_FALSE(isInCheck(s, Color::White));
}

TEST_CASE("CheckDetector: knight gives check", "[check]") {
    BoardState s = kingsOnly();
    // Knight on f3 (rank 2, file 5) attacks e1 (rank 0, file 4)? 
    // Delta: rank -2, file -1 = knight move YES
    s.setCell({2, 5}, KNIGHT, Color::Black);
    CHECK(isInCheck(s, Color::White));
}

TEST_CASE("ChessRules: move that leaves king in check is illegal", "[check]") {
    ChessRules rules;
    BoardState s = kingsOnly();
    s.setCell({0, 3}, ROOK,   Color::White); // white rook on d1
    s.setCell({0, 0}, ROOK,   Color::Black); // black rook on a1 — pins white rook
    // Moving the white rook away from rank 0 would expose king to black rook
    Move m = normalMove({0, 3}, {1, 3}); // rook d1 → d2
    CHECK(rules.validateMove(s, m) == MoveResult::LeavesKingInCheck);
}

TEST_CASE("ChessRules: checkmate detection — fool's mate", "[check]") {
    ChessRules rules;
    BoardState s = rules.initialState();
    // Fool's mate: 1.f3 e5 2.g4 Qh4#
    auto apply = [&](const char* from, const char* to) {
        Move m = normalMove(Position::fromAlgebraic(from),
                            Position::fromAlgebraic(to));
        MoveResult r = rules.validateMove(s, m);
        REQUIRE(r == MoveResult::Legal);
        s = rules.applyMove(s, m);
    };
    apply("f2", "f3");
    apply("e7", "e5");
    apply("g2", "g4");
    apply("d8", "h4"); // Qh4#
    GameStatus st = rules.checkGameStatus(s, Color::White);
    CHECK(st == GameStatus::Checkmate);
}

TEST_CASE("ChessRules: stalemate detection", "[check]") {
    ChessRules rules;
    // Classic stalemate: black king in corner, white queen cuts off all squares
    // Black king a8 (rank 7, file 0), White king c7 (rank 6, file 2), White queen b6 (rank 5, file 1)
    BoardState s;
    s.setCell({7, 0}, KING,  Color::Black);
    s.setCell({6, 2}, KING,  Color::White);
    s.setCell({5, 1}, QUEEN, Color::White);
    s.activePlayer = Color::Black;
    s.castling = CastlingRights{false, false, false, false};

    GameStatus st = rules.checkGameStatus(s, Color::Black);
    CHECK(st == GameStatus::Stalemate);
}

TEST_CASE("CheckDetector: pawn gives check", "[check]") {
    BoardState s = kingsOnly();
    // Black pawn on d2 (rank 1, file 3) — captures diagonally to e1 (rank 0, file 4)
    s.setCell({1, 3}, PAWN, Color::Black);
    CHECK(isInCheck(s, Color::White));
}
