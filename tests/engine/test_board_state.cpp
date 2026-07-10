#include <catch2/catch_test_macros.hpp>
#include "engine/board/BoardState.hpp"
#include "engine/rules/Move.hpp"
#include "chess/ChessPieceTypes.hpp"

using namespace bge;
using namespace angry_chess;

TEST_CASE("BoardState: initial state is empty", "[board]") {
    BoardState s;
    for (int8_t r = 0; r < 8; ++r)
        for (int8_t f = 0; f < 8; ++f)
            CHECK(s.isEmpty({r, f}));
}

TEST_CASE("BoardState: setCell and typeAt", "[board]") {
    BoardState s;
    s.setCell({3, 4}, QUEEN, Color::White);
    CHECK(s.typeAt({3, 4}) == QUEEN);
    CHECK(s.colorAt({3, 4}) == Color::White);
    CHECK_FALSE(s.isEmpty({3, 4}));
}

TEST_CASE("BoardState: clearCell", "[board]") {
    BoardState s;
    s.setCell({0, 0}, ROOK, Color::Black);
    s.clearCell({0, 0});
    CHECK(s.isEmpty({0, 0}));
    CHECK(s.typeAt({0, 0}) == kNoPiece);
    CHECK(s.colorAt({0, 0}) == Color::None);
}

TEST_CASE("BoardState: applyMove basic move", "[board]") {
    BoardState s;
    s.setCell({1, 4}, PAWN, Color::White);
    s.activePlayer = Color::White;

    Move m = normalMove({1, 4}, {3, 4});
    m.flags |= MoveFlag::DoublePush;
    BoardState next = s.applyMove(m);

    CHECK(next.isEmpty({1, 4}));
    CHECK(next.typeAt({3, 4}) == PAWN);
    CHECK(next.colorAt({3, 4}) == Color::White);
    CHECK(next.activePlayer == Color::Black);
    // En-passant target should be set at rank 2, file 4
    REQUIRE(next.enPassantTarget.has_value());
    CHECK(next.enPassantTarget->rank == 2);
    CHECK(next.enPassantTarget->file == 4);
}

TEST_CASE("BoardState: applyMove capture removes enemy piece", "[board]") {
    BoardState s;
    s.setCell({4, 3}, PAWN, Color::White);
    s.setCell({5, 4}, PAWN, Color::Black);
    s.activePlayer = Color::White;

    Move m = captureMove({4, 3}, {5, 4});
    BoardState next = s.applyMove(m);

    CHECK(next.isEmpty({4, 3}));
    CHECK(next.typeAt({5, 4}) == PAWN);
    CHECK(next.colorAt({5, 4}) == Color::White);
}

TEST_CASE("BoardState: applyMove promotion", "[board]") {
    BoardState s;
    s.setCell({6, 0}, PAWN, Color::White);
    s.activePlayer = Color::White;

    Move m;
    m.from          = {6, 0};
    m.to            = {7, 0};
    m.flags         = MoveFlag::Promotion;
    m.promotionType = QUEEN;

    BoardState next = s.applyMove(m);
    CHECK(next.typeAt({7, 0}) == QUEEN);
    CHECK(next.colorAt({7, 0}) == Color::White);
    CHECK(next.isEmpty({6, 0}));
}

TEST_CASE("BoardState: findFirst locates king", "[board]") {
    BoardState s;
    s.setCell({0, 4}, KING, Color::White);
    s.setCell({7, 4}, KING, Color::Black);

    Position wk = s.findFirst(KING, Color::White);
    REQUIRE(wk.isValid());
    CHECK(wk.rank == 0);
    CHECK(wk.file == 4);

    Position bk = s.findFirst(KING, Color::Black);
    REQUIRE(bk.isValid());
    CHECK(bk.rank == 7);
    CHECK(bk.file == 4);
}

TEST_CASE("BoardState: applyMove flips active player", "[board]") {
    BoardState s;
    s.activePlayer = Color::White;
    s.setCell({1, 0}, PAWN, Color::White);

    Move m = normalMove({1, 0}, {2, 0});
    BoardState next = s.applyMove(m);
    CHECK(next.activePlayer == Color::Black);
    BoardState next2 = next.applyMove(normalMove({6, 0}, {5, 0}));
    CHECK(next2.activePlayer == Color::White);
}

TEST_CASE("Position: algebraic notation", "[types]") {
    auto p = Position::fromAlgebraic("e4");
    REQUIRE(p.isValid());
    CHECK(p.rank == 3);
    CHECK(p.file == 4);
    CHECK(p.toAlgebraic() == "e4");

    auto p2 = Position::fromAlgebraic("a1");
    CHECK(p2.rank == 0);
    CHECK(p2.file == 0);
}
