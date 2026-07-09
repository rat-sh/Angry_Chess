#include <catch2/catch_test_macros.hpp>
#include "games/angry_chess/FenParser.hpp"
#include "games/angry_chess/rules/ChessRules.hpp"
#include "games/angry_chess/ChessPieceTypes.hpp"

using namespace bge;
using namespace angry_chess;

TEST_CASE("FenParser: parse starting position", "[fen]") {
    auto s = FenParser::parse(
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    // Verify piece placement
    CHECK(s.typeAt({0, 4}) == KING);
    CHECK(s.colorAt({0, 4}) == Color::White);
    CHECK(s.typeAt({7, 4}) == KING);
    CHECK(s.colorAt({7, 4}) == Color::Black);
    CHECK(s.typeAt({0, 0}) == ROOK);
    CHECK(s.typeAt({0, 1}) == KNIGHT);
    CHECK(s.typeAt({0, 2}) == BISHOP);
    CHECK(s.typeAt({0, 3}) == QUEEN);
    CHECK(s.typeAt({1, 0}) == PAWN);
    CHECK(s.colorAt({1, 0}) == Color::White);
    CHECK(s.typeAt({6, 0}) == PAWN);
    CHECK(s.colorAt({6, 0}) == Color::Black);

    // Metadata
    CHECK(s.activePlayer == Color::White);
    CHECK(s.castling.whiteKingside  == true);
    CHECK(s.castling.whiteQueenside == true);
    CHECK(s.castling.blackKingside  == true);
    CHECK(s.castling.blackQueenside == true);
    CHECK_FALSE(s.enPassantTarget.has_value());
    CHECK(s.halfMoveClock == 0);
    CHECK(s.fullMoveNumber == 1);
}

TEST_CASE("FenParser: parse custom position with en-passant", "[fen]") {
    // After 1.e4 — en-passant target is e3
    auto s = FenParser::parse(
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");

    CHECK(s.activePlayer == Color::Black);
    REQUIRE(s.enPassantTarget.has_value());
    CHECK(s.enPassantTarget->toAlgebraic() == "e3");
    CHECK(s.typeAt({3, 4}) == PAWN); // e4
    CHECK(s.colorAt({3, 4}) == Color::White);
    CHECK(s.isEmpty({1, 4})); // e2 is now empty
}

TEST_CASE("FenParser: generate matches parse round-trip", "[fen]") {
    ChessRules rules;
    BoardState initial = rules.initialState();
    std::string fen = FenParser::generate(initial);
    BoardState parsed = FenParser::parse(fen);

    // Verify every cell matches
    for (int8_t r = 0; r < 8; ++r) {
        for (int8_t f = 0; f < 8; ++f) {
            Position p{r, f};
            CHECK(initial.typeAt(p)  == parsed.typeAt(p));
            CHECK(initial.colorAt(p) == parsed.colorAt(p));
        }
    }
    CHECK(initial.activePlayer == parsed.activePlayer);
}

TEST_CASE("FenParser: legal moves from FEN position (Scholar's mate setup)", "[fen]") {
    // Position after 1.e4 e5 2.Qh5 Nc6 3.Bc4 — Qxf7# is the Scholar's mate threat
    ChessRules rules;
    auto s = FenParser::parse(
        "r1bqkbnr/pppp1ppp/2n5/4p2Q/2B1P3/8/PPPP1PPP/RNB1K1NR b KQkq - 3 3");
    CHECK(s.activePlayer == Color::Black);

    // Black must have legal moves (it's not mate yet)
    auto moves = rules.generateAllLegalMoves(s, Color::Black);
    CHECK(moves.size() > 0);
}

TEST_CASE("FenParser: halfmove clock and full move number", "[fen]") {
    auto s = FenParser::parse(
        "8/8/8/8/8/8/8/4K2k w - - 42 25");
    CHECK(s.halfMoveClock  == 42);
    CHECK(s.fullMoveNumber == 25);
}
