#include "catch.hpp"

#include "token.hpp"

TEST_CASE( "Test Token creation", "[token]" ) {

  Token tko(Token::OPEN);

  REQUIRE(tko.type() == Token::OPEN);
  REQUIRE(tko.asString() == "(");

  Token tkc(Token::CLOSE);

  REQUIRE(tkc.type() == Token::CLOSE);
  REQUIRE(tkc.asString() == ")");

  Token tks("thevalue");

  REQUIRE(tks.type() == Token::SYM);
  REQUIRE(tks.asString() == "thevalue");
}

TEST_CASE( "Test tokenize", "[token]" ) {
  std::string input = R"(
( A a aa )aal ; a comment

(aalii)) 3
)";

  std::istringstream iss(input);

  TokenSequenceType tokens = tokenize(iss);

  REQUIRE(tokens.front().type() == Token::OPEN);
  tokens.pop_front();
  
  REQUIRE(tokens.front().type() == Token::SYM);
  REQUIRE(tokens.front().asString() == "A");
  tokens.pop_front();

  REQUIRE(tokens.front().type() == Token::SYM);
  REQUIRE(tokens.front().asString() == "a");
  tokens.pop_front();

  REQUIRE(tokens.front().type() == Token::SYM);
  REQUIRE(tokens.front().asString() == "aa");
  tokens.pop_front();

  REQUIRE(tokens.front().type() == Token::CLOSE);
  tokens.pop_front();

  REQUIRE(tokens.front().type() == Token::SYM);
  REQUIRE(tokens.front().asString() == "aal");
  tokens.pop_front();

  REQUIRE(tokens.front().type() == Token::OPEN);
  tokens.pop_front();

  REQUIRE(tokens.front().type() == Token::SYM);
  REQUIRE(tokens.front().asString() == "aalii");
  tokens.pop_front();

  REQUIRE(tokens.front().type() == Token::CLOSE);
  tokens.pop_front();

  REQUIRE(tokens.front().type() == Token::CLOSE);
  tokens.pop_front();

  REQUIRE(tokens.front().type() == Token::SYM);
  REQUIRE(tokens.front().asString() == "3");
  tokens.pop_front();

  REQUIRE(tokens.empty());
}

