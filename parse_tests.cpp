#include "catch.hpp"

#include "parse.hpp"

TEST_CASE("Test parser with expected input", "[parse]") {

  std::string program = "(begin (define r 10) (* pi (* r r)))";

  std::istringstream iss(program);

  TokenSequenceType tokens = tokenize(iss);

  REQUIRE(parse(tokens) != Expression());
}

TEST_CASE("Test unbalanced parens", "[parse]") {

  std::string program = "((begin (+ 1))))))";

  std::istringstream iss(program);

  TokenSequenceType tokens = tokenize(iss);

  REQUIRE(parse(tokens) == Expression());
}

TEST_CASE( "Test bad Number literal", "[parse]" ) {

  std::string program = "(define a 1.2abc)";

  std::istringstream iss(program);

  TokenSequenceType tokens = tokenize(iss);

  REQUIRE(parse(tokens) == Expression());
}

TEST_CASE( "Test missing parens", "[parse]" ) {

  std::string program = "+ 1 2";

  std::istringstream iss(program);

  TokenSequenceType tokens = tokenize(iss);

  REQUIRE(parse(tokens) == Expression());
}

TEST_CASE( "Test empty parens", "[parse]" ) {

  std::string program = "()";

  std::istringstream iss(program);

  TokenSequenceType tokens = tokenize(iss);

  REQUIRE(parse(tokens) == Expression());
}

