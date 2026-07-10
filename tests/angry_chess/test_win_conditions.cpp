#include <catch2/catch_test_macros.hpp>
#include "chess/rules/ChessRules.hpp"
#include "chess/ChessPieceTypes.hpp"

using namespace bge;
using namespace angry_chess;

TEST_CASE("Win conditions: fifty-move rule draw", "[win]") {
    ChessRules rules;
    BoardState s;
    s.setCell({0, 4}, KING, Color::White);
    s.setCell({7, 4}, KING, Color::Black);
    s.activePlayer   = Color::White;
    s.castling       = CastlingRights{false, false, false, false};
    s.halfMoveClock  = 100; // 50 full moves = 100 half-moves

    GameStatus st = rules.checkGameStatus(s, Color::White);
    CHECK(st == GameStatus::Draw);
}

TEST_CASE("Win conditions: game ongoing after few moves", "[win]") {
    ChessRules rules;
    BoardState s = rules.initialState();
    GameStatus st = rules.checkGameStatus(s, Color::White);
    CHECK(st == GameStatus::Ongoing);
}

TEST_CASE("Win conditions: check is reported", "[win]") {
    ChessRules rules;
    BoardState s;
    s.setCell({0, 4}, KING,  Color::White);
    s.setCell({7, 4}, KING,  Color::Black);
    s.setCell({0, 0}, ROOK,  Color::Black); // check on white king
    s.activePlayer = Color::White;
    s.castling = CastlingRights{false, false, false, false};

    GameStatus st = rules.checkGameStatus(s, Color::White);
    CHECK(st == GameStatus::Check);
}
