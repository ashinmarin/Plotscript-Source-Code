#include "catch.hpp"

#include "environment.hpp"
#include "semantic_error.hpp"

#include <cmath>
#include <complex>
#include <iostream>

TEST_CASE( "Test default constructor", "[environment]" ) {

  Environment env;

  REQUIRE(env.is_known(Atom("pi")));
  REQUIRE(env.is_exp(Atom("pi")));
  REQUIRE(env.is_known(Atom("e")));
  REQUIRE(env.is_exp(Atom("e")));

  REQUIRE(!env.is_known(Atom("hi")));
  REQUIRE(!env.is_exp(Atom("hi")));

  REQUIRE(env.is_proc(Atom("+")));
  REQUIRE(env.is_proc(Atom("-")));
  REQUIRE(env.is_proc(Atom("*")));
  REQUIRE(env.is_proc(Atom("/")));
  REQUIRE(!env.is_proc(Atom("op")));
}

TEST_CASE( "Test get expression", "[environment]" ) {
  Environment env;

  REQUIRE(env.get_exp(Atom("pi")) == Expression(std::atan2(0, -1)));
  REQUIRE(env.get_exp(Atom("hi")) == Expression());
}

TEST_CASE( "Test add expression", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom("one"), a);
  REQUIRE(env.is_known(Atom("one")));
  REQUIRE(env.is_exp(Atom("one")));
  REQUIRE(env.get_exp(Atom("one")) == a);

  Expression b(Atom("hello"));
  env.add_exp(Atom("hi"), b);
  REQUIRE(env.is_known(Atom("hi")));
  REQUIRE(env.is_exp(Atom("hi")));
  REQUIRE(env.get_exp(Atom("hi")) == b);

  REQUIRE_THROWS_AS(env.add_exp(Atom(1.0), b), SemanticError);
}

TEST_CASE( "Test get built-in procedure", "[environment]" ) {
  Environment env;

  INFO("default procedure")
  Procedure p1 = env.get_proc(Atom("doesnotexist"));
  Procedure p2 = env.get_proc(Atom("alsodoesnotexist"));
  REQUIRE(p1 == p2);
  std::vector<Expression> args;
  REQUIRE(p1(args) == Expression());
  REQUIRE(p2(args) == Expression());
}

TEST_CASE("Testing add procedure", "[environment]") {
	Environment env;
	std::vector<Expression> args;
	
	INFO("trying add procedure: number & number")
	Procedure padd = env.get_proc(Atom("+"));
	args.emplace_back(1.0);
	args.emplace_back(2.0);
	REQUIRE(padd(args) == Expression(3.0));

	args.clear();

	INFO("trying add procedure: complex & complex")
	std::complex<double> aye(0.0, 2.0);
	args.emplace_back(env.get_exp(Atom("I")));
	args.emplace_back(env.get_exp(Atom("I")));
	REQUIRE(padd(args) == Expression(aye));

	args.clear();

	INFO("trying add procedure: number & complex")
	std::complex<double> oy(3.0, 1.0);
	args.emplace_back(1.0);
	args.emplace_back(env.get_exp(Atom("I")));
	args.emplace_back(2.0);
	REQUIRE(padd(args) == Expression(oy));

	args.clear();

	INFO("trying add procedure: wrong argument")
	args.emplace_back(env.get_exp(Atom("A")));
	REQUIRE_THROWS_AS(padd(args), SemanticError);
}

TEST_CASE("Testing sub procedure", "[environment]") {
	Environment env;
	std::vector<Expression> args;
	Procedure psub = env.get_proc(Atom("-"));

	INFO("trying sub procedure: negative number")
	args.emplace_back(1.0);
	REQUIRE(psub(args) == Expression(-1.0));

	args.clear();

	INFO("trying sub procedure: number & number")
	args.emplace_back(1.0);
	args.emplace_back(1.0);
	REQUIRE(psub(args) == Expression(0.0));

	args.clear();

	INFO("trying sub procedure: complex & complex")
	std::complex<double> aye(0.0, 0.0);
	args.emplace_back(env.get_exp(Atom("I")));
	args.emplace_back(env.get_exp(Atom("I")));
	REQUIRE(psub(args) == Expression(aye));

	args.clear();

	INFO("trying sub procedure: number & complex")
	std::complex<double> oy(1.0, -1.0);
	args.emplace_back(1.0);
	args.emplace_back(env.get_exp(Atom("I")));
	REQUIRE(psub(args) == Expression(oy));

	args.clear();

	INFO("trying sub procedure: wrong argument")
	args.emplace_back(env.get_exp(Atom("A")));
	REQUIRE_THROWS_AS(psub(args), SemanticError);

	INFO("trying sub procedure: wrong # of args")
	args.emplace_back(1.0);
	args.emplace_back(1.0);
	REQUIRE_THROWS_AS(psub(args), SemanticError);
}

TEST_CASE("Testing mul procedure", "[environment]") {
	Environment env;
	std::vector<Expression> args;

	INFO("trying mul procedure: number & number")
	Procedure pmul = env.get_proc(Atom("*"));
	args.emplace_back(2.0);
	args.emplace_back(2.0);
	REQUIRE(pmul(args) == Expression(4.0));

	args.clear();

	INFO("trying mul procedure: complex & complex")
	std::complex<double> aye(-1.0, 0.0);
	args.emplace_back(env.get_exp(Atom("I")));
	args.emplace_back(env.get_exp(Atom("I")));
	REQUIRE(pmul(args) == Expression(aye));

	args.clear();

	INFO("trying mul procedure: number & complex")
	std::complex<double> oy(0.0, 4.0);
	args.emplace_back(2.0);
	args.emplace_back(env.get_exp(Atom("I")));
	args.emplace_back(2.0);
	REQUIRE(pmul(args) == Expression(oy));

	args.clear();

	INFO("trying mul procedure: invalid arg")
	args.emplace_back(env.get_exp(Atom("A")));
	REQUIRE_THROWS_AS(pmul(args), SemanticError);
}

TEST_CASE("Testing div procedure", "[environment]") {
	Environment env;
	std::vector<Expression> args;

	INFO("trying div procedure: number & number")
	Procedure pdiv = env.get_proc(Atom("/"));
	args.emplace_back(3.0);
	args.emplace_back(2.0);
	REQUIRE(pdiv(args) == Expression(1.5));

	args.clear();

	INFO("trying div procedure: complex & complex")
	std::complex<double> aye(1.0, 0.0);
	args.emplace_back(env.get_exp(Atom("I")));
	args.emplace_back(env.get_exp(Atom("I")));
	REQUIRE(pdiv(args) == Expression(aye));

	args.clear();

	INFO("trying div procedure: number & complex")
	std::complex<double> oy(0.0, 0.5);
	args.emplace_back(env.get_exp(Atom("I")));
	args.emplace_back(2.0);
	REQUIRE(pdiv(args) == Expression(oy));

	args.clear();

	INFO("trying div procedure: wrong argument")
	args.emplace_back(env.get_exp(Atom("A")));
	REQUIRE_THROWS_AS(pdiv(args), SemanticError);

	INFO("trying div procedure: wrong # of args")
	args.emplace_back(1.0);
	args.emplace_back(1.0);
	REQUIRE_THROWS_AS(pdiv(args), SemanticError);
}

TEST_CASE("Test sqrt", "[environment]") {
	Environment env;
	std::vector<Expression> args;
	Procedure psq = env.get_proc(Atom("sqrt"));

	INFO("trying sqrt procedure: number")
	args.emplace_back(4.0);
	REQUIRE(psq(args) == Expression(2.0));

	args.clear();

	INFO("trying sqrt procedure: negative number")
	std::complex<double> aye(0.0, 1.0);
	args.emplace_back(-1.0);
	REQUIRE(psq(args) == Expression(aye));

	args.clear();

	INFO("trying sqrt procedure: complex")
	aye = std::sqrt(aye);
	args.emplace_back(env.get_exp(Atom("I")));
	REQUIRE(psq(args) == Expression(aye));

	args.clear();

	INFO("trying sqrt procedure: invalid arg")
	args.emplace_back(env.get_exp(Atom("!")));
	REQUIRE_THROWS_AS(psq(args), SemanticError);

	INFO("trying sqrt procedure: wrong # args")
	args.emplace_back(1.0);
	REQUIRE_THROWS_AS(psq(args), SemanticError);
}

TEST_CASE("Test exponential", "[environment]") {
	Environment env;
	std::vector<Expression> args;
	Procedure pexp = env.get_proc(Atom("^"));

	INFO("trying expo procedure: number & number")
	args.emplace_back(2.0);
	args.emplace_back(2.0);
	REQUIRE(pexp(args) == Expression(4.0));

	args.clear();

	INFO("trying expo procedure: complex & complex")
	std::complex<double> aye(0.0, 1.0);
	args.emplace_back(aye);
	args.emplace_back(aye);
	aye = std::pow(aye, aye);
	REQUIRE(pexp(args) == Expression(aye));

	args.clear();

	INFO("trying expo procedure: number & complex")
	aye = std::complex<double>(-1,0);
	args.emplace_back(env.get_exp(Atom("I")));
	args.emplace_back(2.0);
	REQUIRE(pexp(args) == Expression(aye));

	args.clear();

	INFO("trying expo procedure: invalid arg")
	args.emplace_back(env.get_exp(Atom("!")));
	REQUIRE_THROWS_AS(pexp(args), SemanticError);

	INFO("trying expo procedure: wrong # args")
	args.emplace_back(1.0);
	args.emplace_back(2.0);
	REQUIRE_THROWS_AS(pexp(args), SemanticError);
}

TEST_CASE("Test ln", "[environment]") {
	Environment env;
	std::vector<Expression> args;
	Procedure pln = env.get_proc(Atom("ln"));

	INFO("trying ln procedure: number")
	args.emplace_back(1);
	REQUIRE(pln(args) == Expression(0));

	args.clear();

	INFO("trying ln procedure: complex")
	std::complex<double> aye(0.0, 1.0);
	args.emplace_back(aye);
	REQUIRE(pln(args) == Expression(std::log(aye)));

	args.clear();

	INFO("trying ln procedure: negative")
	std::complex<double> neg(-1.0, 0.0);
	args.emplace_back(-1);
	REQUIRE(pln(args) == Expression(std::log(neg)));

	INFO("trying ln procedure: wrong # of args")
	args.emplace_back(-2);
	REQUIRE_THROWS_AS(pln(args), SemanticError);
}

TEST_CASE("Test sin", "[environment]") {
	Environment env;
	std::vector<Expression> args;
	Procedure psin = env.get_proc(Atom("sin"));

	INFO("trying sin procedure: number")
	args.emplace_back(1);
	REQUIRE(psin(args) == Expression(std::sin(1)));

	args.clear();

	INFO("trying sin procedure: complex")
	std::complex<double> aye(0.0, 1.0);
	args.emplace_back(aye);
	REQUIRE(psin(args) == Expression(std::sin(aye)));

	args.clear();

	INFO("trying sin procedure: invalid arg")
	args.emplace_back(Atom("aaa"));
	REQUIRE_THROWS_AS(psin(args), SemanticError);

	INFO("trying sin procedure: wrong # of args")
	args.emplace_back(-2);
	REQUIRE_THROWS_AS(psin(args), SemanticError);
}


TEST_CASE("Test cos", "[environment]") {
	Environment env;
	std::vector<Expression> args;
	Procedure pcos = env.get_proc(Atom("cos"));

	INFO("trying cos procedure: number")
	args.emplace_back(1);
	REQUIRE(pcos(args) == Expression(std::cos(1)));

	args.clear();

	INFO("trying cos procedure: complex")
	std::complex<double> aye(0.0, 1.0);
	args.emplace_back(aye);
	REQUIRE(pcos(args) == Expression(std::cos(aye)));

	args.clear();

	INFO("trying cos procedure: invalid arg")
	args.emplace_back(Atom("aaa"));
	REQUIRE_THROWS_AS(pcos(args), SemanticError);

	INFO("trying cos procedure: wrong # of args")
	args.emplace_back(-2);
	REQUIRE_THROWS_AS(pcos(args), SemanticError);
}

TEST_CASE("Test tan", "[environment]") {
	Environment env;
	std::vector<Expression> args;
	Procedure ptan = env.get_proc(Atom("tan"));

	INFO("trying tan procedure: number")
	args.emplace_back(1);
	REQUIRE(ptan(args) == Expression(std::tan(1)));

	args.clear();

	INFO("trying tan procedure: complex")
	std::complex<double> aye(0.0, 1.0);
	args.emplace_back(aye);
	REQUIRE(ptan(args) == Expression(std::tan(aye)));

	args.clear();

	INFO("trying tan procedure: invalid arg")
	args.emplace_back(Atom("aaa"));
	REQUIRE_THROWS_AS(ptan(args), SemanticError);

	INFO("trying tan procedure: wrong # of args")
	args.emplace_back(-2);
	REQUIRE_THROWS_AS(ptan(args), SemanticError);
}

TEST_CASE("Test real", "[environment]") {
	Environment env;
	std::vector<Expression> args;
	Procedure preal = env.get_proc(Atom("real"));
	std::complex<double> aye(2.0, 1.0);

	INFO("trying real procedure")
	args.emplace_back(aye);
	REQUIRE(preal(args) == Expression(2.0));

	args.clear();

	INFO("trying real procedure: invalid argument")
	args.emplace_back(1.0);
	REQUIRE_THROWS_AS(preal(args), SemanticError);

	INFO("trying real procedure: invalid number of args")
	args.emplace_back(2.0);
	REQUIRE_THROWS_AS(preal(args), SemanticError);
}

TEST_CASE("Test imag", "[environment]") {
	Environment env;
	std::vector<Expression> args;
	Procedure pimag = env.get_proc(Atom("imag"));
	std::complex<double> aye(2.0, 1.0);

	INFO("trying imag procedure")
	args.emplace_back(aye);
	REQUIRE(pimag(args) == Expression(1.0));

	args.clear();

	INFO("trying imag procedure: invalid argument")
	args.emplace_back(1.0);
	REQUIRE_THROWS_AS(pimag(args), SemanticError);

	INFO("trying imag procedure: invalid number of args")
	args.emplace_back(2.0);
	REQUIRE_THROWS_AS(pimag(args), SemanticError);
}

TEST_CASE("Test mag", "[environment]") {
	Environment env;
	std::vector<Expression> args;
	Procedure pmag = env.get_proc(Atom("mag"));
	std::complex<double> aye(2.0, 4.0);

	INFO("trying mag procedure")
	args.emplace_back(aye);
	REQUIRE(pmag(args) == Expression(std::abs(aye)));

	args.clear();

	INFO("trying mag procedure: invalid argument")
	args.emplace_back(1.0);
	REQUIRE_THROWS_AS(pmag(args), SemanticError);

	INFO("trying mag procedure: invalid number of args")
	args.emplace_back(2.0);
	REQUIRE_THROWS_AS(pmag(args), SemanticError);
}

TEST_CASE("Test arg", "[environment]") {
	Environment env;
	std::vector<Expression> args;
	Procedure parg = env.get_proc(Atom("arg"));
	std::complex<double> aye(2.0, 4.0);

	INFO("trying arg procedure")
	args.emplace_back(aye);
	REQUIRE(parg(args) == Expression(std::arg(aye)));

	args.clear();

	INFO("trying arg procedure: invalid argument")
	args.emplace_back(1.0);
	REQUIRE_THROWS_AS(parg(args), SemanticError);

	INFO("trying arg procedure: invalid number of args")
	args.emplace_back(2.0);
	REQUIRE_THROWS_AS(parg(args), SemanticError);
}

TEST_CASE("Test conj", "[environment]") {
	Environment env;
	std::vector<Expression> args;
	Procedure pconj = env.get_proc(Atom("conj"));
	std::complex<double> aye(2.0, 4.0);

	INFO("trying conj procedure")
	args.emplace_back(aye);
	REQUIRE(pconj(args) == Expression(std::conj(aye)));

	args.clear();

	INFO("trying conj procedure: invalid argument")
	args.emplace_back(1.0);
	REQUIRE_THROWS_AS(pconj(args), SemanticError);

	INFO("trying conj procedure: invalid number of args")
	args.emplace_back(2.0);
	REQUIRE_THROWS_AS(pconj(args), SemanticError);
}

TEST_CASE("Test list", "[environment]") {
	Environment env;
	std::vector<Expression> args;
	Procedure plist = env.get_proc(Atom("list"));
	Expression list1(Atom("islist"));
	list1.setList();
	
	std::vector<Expression> list2;
	std::complex<double> aye(0.0, 1.0);

	INFO("trying list procedure: adding");
	args.emplace_back(1.0);
	args.emplace_back(aye);
	list2.emplace_back(1.0);
	list2.emplace_back(aye);
	list1.setTail(list2);
	REQUIRE(plist(args) == list1);

	args.clear();
	list2.clear();

	INFO("trying list procedure: nested");
	std::vector<Expression> args1;
	std::vector<Expression> list3;
	Expression exp_list3(Atom("islist"));
	list3.emplace_back(1.0);
	exp_list3.setTail(list3);
	exp_list3.setList();
	args1.emplace_back(exp_list3);
	Expression x1(Atom("islist"));
	x1.setList();
	std::vector<Expression> y1;
	y1.emplace_back(exp_list3);
	x1.setTail(y1);
	REQUIRE(plist(args1) == x1);

	INFO("trying empty list");
	std::vector<Expression> list4;
	Expression exp(Atom("islist"));
	exp.setList();
	exp.setTail(list4);
	args.clear();
	REQUIRE(plist(args) == exp);
}

TEST_CASE("Test first", "[environment]") {
	Environment env;
	std::vector<Expression> args;
	Procedure pfirst = env.get_proc(Atom("first"));
	std::vector<Expression> list1;
	Expression exp(Atom("tolist"));
	exp.setList();

	INFO("trying first procedure");
	list1.emplace_back(1.0);
	list1.emplace_back(2.0);
	exp.setTail(list1);
	args.emplace_back(exp);
	REQUIRE(pfirst(args) == Expression(1.0));

	args.clear();

	INFO("trying first procedure: too many arguments");
	args.emplace_back(exp);
	args.emplace_back(exp);
	REQUIRE_THROWS_AS(pfirst(args), SemanticError);

	args.clear();

	INFO("trying first procedure: invalid argument");
	args.emplace_back(1.0);
	REQUIRE_THROWS_AS(pfirst(args), SemanticError);

	args.clear();
	list1.clear();

	INFO("trying first procedure: empty list");
	REQUIRE_THROWS_AS(pfirst(args), SemanticError);
}

TEST_CASE("Test rest", "[environment]") {
	Environment env;
	std::vector<Expression> args;
	Procedure prest = env.get_proc(Atom("rest"));
	std::vector<Expression> list1;
	Expression exp(Atom("islist"));
	exp.setList();
	std::vector<Expression> empty_list;
	Expression exp1(Atom("islist"));
	exp1.setList();
	exp1.setTail(empty_list);

	INFO("trying rest procedure: one member");
	list1.emplace_back(1.0);
	exp.setTail(list1);
	args.emplace_back(exp);
	REQUIRE(prest(args) == exp1);
	
	args.clear();

	INFO("trying rest procedure: two members");
	list1.emplace_back(2.0);
	exp.setTail(list1);
	args.emplace_back(exp);
	Expression exp2(Atom("islist"));
	exp2.setList();
	std::vector<Expression> l;
	l.emplace_back(2.0);
	exp2.setTail(l);
	REQUIRE(prest(args) == exp2);

	args.clear();

	INFO("trying rest procedure: too many arguments");
	args.emplace_back(exp);
	args.emplace_back(exp);
	REQUIRE_THROWS_AS(prest(args), SemanticError);

	args.clear();

	INFO("trying rest procedure: invalid argument");
	args.emplace_back(1.0);
	REQUIRE_THROWS_AS(prest(args), SemanticError);

	args.clear();
	list1.clear();

	INFO("trying rest procedure: empty list");
	args.emplace_back(exp1);
	REQUIRE_THROWS_AS(prest(args), SemanticError);
}

TEST_CASE("Test length", "[environment]") {
	Environment env;
	std::vector<Expression> args;
	Procedure plength = env.get_proc(Atom("length"));
	std::vector<Expression> list1;
	Expression exp1(Atom("list"));

	INFO("testing length");
	list1.emplace_back(1.0);
	list1.emplace_back(2.0);
	exp1.setTail(list1);
	exp1.setList();
	args.emplace_back(exp1);
	REQUIRE(plength(args) == Expression(2.0));

	args.clear();

	INFO("testing length procedure: invalid argument");
	args.emplace_back(1.0);
	REQUIRE_THROWS_AS(plength(args), SemanticError);

	INFO("testing length procedure: too many arguments");
	args.emplace_back(2.0);
	REQUIRE_THROWS_AS(plength(args), SemanticError);
}

TEST_CASE("Test append", "[environment]") {
	Environment env;
	std::vector<Expression> args;
	Procedure pappend = env.get_proc(Atom("append"));
	std::vector<Expression> list1;
	std::vector<Expression> empty_list;
	std::vector<Expression> v1;
	Expression empty(Atom("islist"));
	empty.setTail(empty_list);
	empty.setList();
	std::complex<double> aye(0.0, 1.0);

	INFO("trying append procedure");
	args.emplace_back(empty);
	list1.emplace_back(1.0);
	list1.emplace_back(aye);
	list1.emplace_back(empty);

	Expression exp1(Atom("islist"));
	exp1.setTail(list1);
	exp1.setList();
	v1 = { exp1 };
	args.emplace_back(exp1);
	Expression exp2(Atom("islist"));

	exp2.setTail(v1);
	exp2.setList();
	REQUIRE(pappend(args) == exp2);

	args.clear();

	INFO("trying append procedure: invalid number of arguments");
	args.emplace_back(exp1);
	REQUIRE_THROWS_AS(pappend(args), SemanticError);

	args.clear();

	INFO("trying append procedure: not a list");
	args.emplace_back(1.0);
	args.emplace_back(1.0);
	REQUIRE_THROWS_AS(pappend(args), SemanticError);
}

TEST_CASE("Test join", "[environment]") {
	Environment env;
	std::vector<Expression> args;
	Procedure pjoin = env.get_proc(Atom("join"));
	std::vector<Expression> list1;
	list1.emplace_back(1.0);
	list1.emplace_back(2.0);

	std::vector<Expression> list2;
	list2.emplace_back(3.0);
	list2.emplace_back(4.0);

	std::vector<Expression> list3;
	list3.emplace_back(1.0);
	list3.emplace_back(2.0);
	list3.emplace_back(3.0);
	list3.emplace_back(4.0);

	Expression exp1(Atom("islist"));
	exp1.setTail(list1);
	exp1.setList();
	Expression exp2(Atom("islist"));
	exp2.setTail(list2);
	exp2.setList();
	Expression exp3(Atom("islist"));
	exp3.setTail(list3);
	exp3.setList();

	INFO("testing join procedure");
	args.emplace_back(exp1);
	args.emplace_back(exp2);
	REQUIRE(pjoin(args) == Expression(exp3));

	args.clear();

	INFO("testing join procedure: invalid number of arguments");
	args.emplace_back(exp1);
	REQUIRE_THROWS_AS(pjoin(args), SemanticError);
	
	INFO("testing join procedure: invalid second argument");
	args.emplace_back(1.0);
	REQUIRE_THROWS_AS(pjoin(args), SemanticError);
}

TEST_CASE( "Test reset", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom("one"), a);
  Expression b(Atom("hello"));
  env.add_exp(Atom("hi"), b);

  env.reset();
  REQUIRE(!env.is_known(Atom("one")));
  REQUIRE(!env.is_exp(Atom("one")));
  REQUIRE(env.get_exp(Atom("one")) == Expression());
  REQUIRE(!env.is_known(Atom("hi")));
  REQUIRE(!env.is_exp(Atom("hi")));
  REQUIRE(env.get_exp(Atom("hi")) == Expression());
}

TEST_CASE( "Test semeantic errors", "[environment]" ) {

  Environment env;

  {
    Expression exp(Atom("begin"));
    
    REQUIRE_THROWS_AS(exp.eval(env), SemanticError);
  }
}

