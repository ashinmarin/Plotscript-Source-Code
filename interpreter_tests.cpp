#include "catch.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "semantic_error.hpp"
#include "interpreter.hpp"
#include "expression.hpp"

Expression run(const std::string & program){
  
  std::istringstream iss(program);
    
  Interpreter interp;
    
  bool ok = interp.parseStream(iss);
  if(!ok){
    std::cerr << "Failed to parse: " << program << std::endl; 
  }
  REQUIRE(ok == true);

  Expression result;
  REQUIRE_NOTHROW(result = interp.evaluate());

  return result;
}

TEST_CASE( "Test Interpreter parser with expected input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r)))";

  std::istringstream iss(program);
 
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == true);
}

TEST_CASE( "Test Interpreter parser with numerical literals", "[interpreter]" ) {

  std::vector<std::string> programs = {"(1)", "(+1)", "(+1e+0)", "(1e-0)"};
  
  for(auto program : programs){
    std::istringstream iss(program);
 
    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == true);
  }

  {
    std::istringstream iss("(define x 1abc)");
    
    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with truncated input", "[interpreter]" ) {

  {
    std::string program = "(f";
    std::istringstream iss(program);
  
    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }
  
  {
    std::string program = "(begin (define r 10) (* pi (* r r";
    std::istringstream iss(program);

    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with extra input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r))) )";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with single non-keyword", "[interpreter]" ) {

  std::string program = "hello";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty input", "[interpreter]" ) {

  std::string program;
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty expression", "[interpreter]" ) {

  std::string program = "( )";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with bad number string", "[interpreter]" ) {

  std::string program = "(1abc)";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with incorrect input. Regression Test", "[interpreter]" ) {

  std::string program = "(+ 1 2) (+ 3 4)";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter result with literal expressions", "[interpreter]" ) {
  
  { // Number
    std::string program = "(4)";
    Expression result = run(program);
    REQUIRE(result == Expression(4.));
  }

  { // Symbol
    std::string program = "(pi)";
    Expression result = run(program);
    REQUIRE(result == Expression(atan2(0, -1)));
  }

}

TEST_CASE( "Test Interpreter result with simple procedures (add)", "[interpreter]" ) {

  { // add, binary case
    std::string program = "(+ 1 2)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(3.));
  }
  
  { // add, 3-ary case
    std::string program = "(+ 1 2 3)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(6.));
  }

  { // add, 6-ary case
    std::string program = "(+ 1 2 3 4 5 6)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(21.));
  }
}
  
TEST_CASE( "Test Interpreter special forms: begin and define", "[interpreter]" ) {

  {
    std::string program = "(define answer 42)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }

  {
    std::string program = "(begin (define answer 42)\n(answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }
  
  {
    std::string program = "(begin (define answer (+ 9 11)) (answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(20.));
  }

  {
    std::string program = "(begin (define a 1) (define b 1) (+ a b))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(2.));
  }
}

TEST_CASE( "Test a medium-sized expression", "[interpreter]" ) {

  {
    std::string program = "(+ (+ 10 1) (+ 30 (+ 1 1)))";
    Expression result = run(program);
    REQUIRE(result == Expression(43.));
  }
}

TEST_CASE( "Test arithmetic procedures", "[interpreter]" ) {

  {
    std::vector<std::string> programs = {"(+ 1 -2)",
					 "(+ -3 1 1)",
					 "(- 1)",
					 "(- 1 2)",
					 "(* 1 -1)",
					 "(* 1 1 -1)",
					 "(/ -1 1)",
					 "(/ 1 -1)"};

    for(auto s : programs){
      Expression result = run(s);
      REQUIRE(result == Expression(-1.));
    }
  }
}


TEST_CASE( "Test some semantically invalid expresions", "[interpreter]" ) {
  
  std::vector<std::string> programs = {"(@ none)", // so such procedure
				       "(- 1 1 2)", // too many arguments
				       "(define begin 1)"}; // redefine special form
    for(auto s : programs){
      Interpreter interp;

      std::istringstream iss(s);
      
      bool ok = interp.parseStream(iss);
      REQUIRE(ok == true);
      
      REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
    }
}

TEST_CASE( "Test for exceptions from semantically incorrect input", "[interpreter]" ) {

  std::string input = R"(
(+ 1 a)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test malformed define", "[interpreter]" ) {

    std::string input = R"(
(define a 1 2)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test using number as procedure", "[interpreter]" ) {
    std::string input = R"(
(1 2 3)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("testing range for lists", "[interpreter]") {
	std::string input = "(range 0 5 1)";
	Interpreter interp;
	std::istringstream iss(input);
	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);

	Expression exp = interp.evaluate();
	std::ostringstream o;
	o << exp;
	REQUIRE(o.str() == "((0) (1) (2) (3) (4) (5))");

	input = "(range 0 5)";
	std::istringstream iss1(input);
	ok = interp.parseStream(iss1);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	input = "(range 0 5 ^)";
	std::istringstream iss2(input);
	ok = interp.parseStream(iss2);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	input = "(range 5 0 1)";
	std::istringstream iss3(input);
	ok = interp.parseStream(iss3);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	input = "(range 0 5 0)";
	std::istringstream iss4(input);
	ok = interp.parseStream(iss4);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("testing lambda", "[interpreter]") {
	Interpreter interp;
	std::string input = "(define a (lambda (x) (+ 1 2)))";
	std::istringstream iss(input);
	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression exp = interp.evaluate();
	std::ostringstream o;
	o << exp;
	REQUIRE(o.str() == "(((x)) (+ (1) (2)))");

	input = "(lambda (x))";
	std::istringstream iss1(input);
	ok = interp.parseStream(iss1);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	input = "(a 1 2)";
	std::istringstream iss2(input);
	ok = interp.parseStream(iss2);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	input = "(a 1)";
	std::istringstream iss3(input);
	ok = interp.parseStream(iss3);
	REQUIRE(ok == true);
	exp = interp.evaluate();
	std::ostringstream o1;
	o1 << exp;
	REQUIRE(o1.str() == "(3)");
}

TEST_CASE("testing apply", "[interpreter]") {
	Interpreter interp;
	std::string input = "(apply + (list 1 2 3 4))";
	std::istringstream iss(input);
	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression exp = interp.evaluate();
	std::ostringstream o;
	o << exp;
	REQUIRE(o.str() == "(10)");

	input = "(apply +)";
	std::istringstream iss1(input);
	ok = interp.parseStream(iss1);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	input = "(apply ! !)";
	std::istringstream iss2(input);
	ok = interp.parseStream(iss2);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	input = "(apply + 1)";
	std::istringstream iss3(input);
	ok = interp.parseStream(iss3);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("testing map", "[interpreter]") {
	Interpreter interp;
	std::string input = "(map / (list 1 2 4))";
	std::istringstream iss(input);
	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression exp = interp.evaluate();
	std::ostringstream o;
	o << exp;
	REQUIRE(o.str() == "((1) (0.5) (0.25))");

	input = "(map +)";
	std::istringstream iss1(input);
	ok = interp.parseStream(iss1);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	input = "(map ! !)";
	std::istringstream iss2(input);
	ok = interp.parseStream(iss2);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	input = "(map + 1)";
	std::istringstream iss3(input);
	ok = interp.parseStream(iss3);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("testing set-property", "[interpreter]") {
	Interpreter interp;
	std::string input = "(set-property \"number\" \"three\" (3))";
	std::istringstream iss(input);
	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression exp = interp.evaluate();
	std::ostringstream o;
	o << exp;
	REQUIRE(o.str() == "(3)");

	input = "(set-property 1)";
	std::istringstream iss1(input);
	ok = interp.parseStream(iss1);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	input = "(set-property 1 2 3)";
	std::istringstream iss2(input);
	ok = interp.parseStream(iss2);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("testing get-property", "[interpreter]") {
	Interpreter interp;
	std::string input = "(get-property \"foo\" (set-property \"foo\" \"foo1\" (3)))";
	std::istringstream iss(input);
	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression exp = interp.evaluate();
	std::ostringstream o;
	o << exp;
	REQUIRE(o.str() == "(\"foo1\")");
	
	input = "(get-property 1)";
	std::istringstream iss1(input);
	ok = interp.parseStream(iss1);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	input = "(get-property 1 2)";
	std::istringstream iss2(input);
	ok = interp.parseStream(iss2);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("testing parse again", "[interpreter]") {
	Interpreter interp;
	std::string input = "(define a (\"foo\"))";
	std::istringstream iss(input);
	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression exp = interp.evaluate();
	std::ostringstream o;
	o << exp;
	REQUIRE(o.str() == "(\"foo\")");

	input = "\"foo\"";
	std::istringstream iss1(input);
	ok = interp.parseStream(iss1);
	REQUIRE(ok == false);


}

TEST_CASE("extra environment tests", "[interpreter]") {
	Interpreter interp;
	std::string input = "(- I)";
	std::istringstream iss(input);
	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression exp = interp.evaluate();
	std::ostringstream o;
	o << exp;
	REQUIRE(o.str() == "(-0,-1)");

	input = "(- I 1)";
	std::istringstream iss1(input);
	ok = interp.parseStream(iss1);
	REQUIRE(ok == true);
	exp = interp.evaluate();
	std::ostringstream o1;
	o1 << exp;
	REQUIRE(o1.str() == "(-1,1)");

	input = "(- I !)";
	std::istringstream iss2(input);
	ok = interp.parseStream(iss2);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	// division
	input = "(/ I)";
	std::istringstream iss3(input);
	ok = interp.parseStream(iss3);
	REQUIRE(ok == true);
	exp = interp.evaluate();
	std::ostringstream o2;
	o2 << exp;
	REQUIRE(o2.str() == "(0,-1)");

	input = "(/ 1 I)";
	std::istringstream iss4(input);
	ok = interp.parseStream(iss4);
	REQUIRE(ok == true);
	exp = interp.evaluate();
	std::ostringstream o3;
	o3 << exp;
	REQUIRE(o3.str() == "(0,-1)");

	input = "(/ I !)";
	std::istringstream iss5(input);
	ok = interp.parseStream(iss5);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	// expo
	input = "(^ 1 I)";
	std::istringstream iss6(input);
	ok = interp.parseStream(iss6);
	REQUIRE(ok == true);
	exp = interp.evaluate();
	std::ostringstream o4;
	o4 << exp;
	REQUIRE(o4.str() == "(1,0)");

	input = "(^ I !)";
	std::istringstream iss7(input);
	ok = interp.parseStream(iss7);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	// ln
	input = "(ln !)";
	std::istringstream iss8(input);
	ok = interp.parseStream(iss8);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("testing expression", "[interpreter]") {
	Interpreter interp;
	std::string input = "(begin)";
	std::istringstream iss(input);
	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	input = "(define + 1)";
	std::istringstream iss2(input);
	ok = interp.parseStream(iss2);
	REQUIRE(ok == true);

	input = "(list)";
	std::istringstream iss3(input);
	ok = interp.parseStream(iss3);
	REQUIRE(ok == true);

}

TEST_CASE("testing gui stuff", "[interpreter]") {
	Interpreter interp;
	std::string program = "(begin (define f (lambda (x) (list x (+ (* 2 x) 1)))) (discrete-plot (map f (range -2 2 0.5)) (list (list \"title\" \"The Data\") (list \"abscissa-label\" \"X Label\") (list \"ordinate-label\" \"Y Label\") (list \"text-scale\" 1))))";
	std::istringstream iss(program);
	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression exp = interp.evaluate();
	REQUIRE(exp.getTail().size() == 31);

	program = "(begin (define f (lambda(x) (sin x))) (continuous-plot f (list (- pi) pi)))";
	std::istringstream iss1(program);
	ok = interp.parseStream(iss1);
	REQUIRE(ok == true);
	exp = interp.evaluate();

	program = "(begin (define f (lambda (x) (+ (* 2 x) 1))) (continuous-plot f (list -2 2) (list (list \"title\" \"A continuous linear function\") (list \"abscissa-label\" \"x\") (list \"ordinate-label\" \"y\") (list \"text-scale\" 1))))";
	std::istringstream iss2(program);
	ok = interp.parseStream(iss2);
	REQUIRE(ok == true);
	exp = interp.evaluate();
}