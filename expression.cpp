#include "expression.hpp"
#include "expression.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>

#include "environment.hpp"
#include "semantic_error.hpp"

#include <atomic>
std::atomic<bool> interrupt;

Expression::Expression() {}

Expression::Expression(const Atom & a) {
	m_head = a;
}

Expression::Expression(const Expression & a, const Expression & b) {
	m_head = Atom("lambda");

	// want to make constructor with tail that has a list as first expression and the procedure as the second expression
	std::vector<Expression> tmp = { a, b };
	m_tail = tmp;
}

// recursive copy
Expression::Expression(const Expression & a) {
	m_head = a.m_head;
	for (auto e : a.m_tail) {
		m_tail.push_back(e);
	}
	m_properties.insert(a.m_properties.begin(), a.m_properties.end());
	is_list = a.is_list;
}

Expression & Expression::operator=(const Expression & a) {
	// prevent self-assignment
	if (this != &a) {
		m_head = a.m_head;
		m_tail.clear();
		for (auto e : a.m_tail) {
			m_tail.push_back(e);
		}
		m_properties.insert(a.m_properties.begin(), a.m_properties.end());
		is_list = a.is_list;
	}

	return *this;
}

Atom & Expression::head() {
	return m_head;
}

const Atom & Expression::head() const {
	return m_head;
}

bool Expression::isHeadComplex() const noexcept {
	return m_head.isComplex();
}

bool Expression::isHeadNumber() const noexcept {
	return m_head.isNumber();
}

bool Expression::isHeadSymbol() const noexcept {
	return m_head.isSymbol();
}

void Expression::append(const Atom & a) {
	m_tail.emplace_back(a);
}

void Expression::append(const Expression & exp) {
	m_tail.emplace_back(exp);
}

Expression * Expression::tail() {
	Expression * ptr = nullptr;

	if (m_tail.size() > 0) {
		ptr = &m_tail.back();
	}

	return ptr;
}

bool Expression::isTailEmpty() const noexcept
{
	return (m_tail.size() == 0);
}

void Expression::setTail(std::vector<Expression> to_add)
{
	m_tail = to_add;
}

std::vector<Expression> Expression::getTail() const noexcept {
	return m_tail;
}

Expression::ConstIteratorType Expression::tailConstBegin() const noexcept {
	return m_tail.cbegin();
}

Expression::ConstIteratorType Expression::tailConstEnd() const noexcept {
	return m_tail.cend();
}

Expression apply(const Atom & op, const std::vector<Expression> & args, const Environment & env) {

	// head must be a symbol
	if (!op.isSymbol()) {
		throw SemanticError("Error during evaluation: procedure name not symbol");
	}

	// must map to a proc
	if (!env.is_proc(op)) {
		throw SemanticError("Error during evaluation: symbol does not name a procedure");
	}

	// map from symbol to proc
	Procedure proc = env.get_proc(op);

	// call proc with args
	return proc(args);
}

Expression Expression::handle_lookup(const Atom & head, const Environment & env) {
	if (head.isString()) {
		return Expression(head);
	}
	else if (head.isSymbol()) { // if symbol is in env return value
		if (env.is_exp(head)) {
			return env.get_exp(head);
		}
		else {
			throw SemanticError("Error during evaluation: unknown symbol");
		}
	}
	else if (head.isNumber() || head.isComplex()) {
		return Expression(head);
	}
	else {
		throw SemanticError("Error during evaluation: Invalid type in terminal expression");
	}
}

Expression Expression::handle_begin(Environment & env) {

	if (m_tail.size() == 0) {
		throw SemanticError("Error during evaluation: zero arguments to begin");
	}

	// evaluate each arg from tail, return the last
	Expression result;
	for (Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it) {
		result = it->eval(env);
	}

	return result;
}

Expression Expression::handle_define(Environment & env) {

	// tail must have size 3 or error
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of arguments to define");
	}

	// tail[0] must be symbol
	if (!m_tail[0].isHeadSymbol()) {
		throw SemanticError("Error during evaluation: first argument to define not symbol");
	}

	// but tail[0] must not be a special-form or procedure
	std::string s = m_tail[0].head().asSymbol();
	if ((s == "define") || (s == "begin") || (s == "lambda")) {
		throw SemanticError("Error during evaluation: attempt to redefine a special-form");
	}

	if (env.is_proc(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
	}

	// eval tail[1]
	Expression result = m_tail[1].eval(env);

	if (env.is_exp(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
	}

	// and add to env
	env.add_exp(m_tail[0].head(), result);

	return result;
}

Expression Expression::handle_lambda() {
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of arguments to lambda");
	}

	// initialize lambda with arguments
	std::vector<Expression> args = exp_to_atom(m_tail[0]);
	Expression tmp;
	tmp.setTail(args);
	tmp.setList();
	Expression result(tmp, m_tail[1]);
	return result;
}

// helper function that converts an expression into a vector of atoms
std::vector<Expression> Expression::exp_to_atom(Expression e) {
	std::vector<Expression> result;
	result.push_back(e.head());
	for (ConstIteratorType it = e.tailConstBegin(); it != e.tailConstEnd(); ++it) {
		result.push_back((it->head()));
	}
	return result;
}

Expression Expression::handle_recall_lambda(Environment &env) {
	std::vector<Expression> results;
	for (Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it) {
		results.push_back(it->eval(env));
	}
	// create lambda environment
	Environment lambda_env(env);
	Expression result = env.get_exp(m_head);

	// getting args and procedure
	Expression lambda_a = *(result.tailConstBegin());
	Expression lambda_e = *(result.tailConstBegin() + 1);

	// converting to vector for convenience
	std::vector<Expression> l_args = lambda_a.getTail();

	// checking that number of args is the same
	if (l_args.size() != results.size()) {
		throw SemanticError("Error: incorrect number of arguments to lambda");
	}

	// defining arguments in lambda env
	for (unsigned int i = 0; i < l_args.size(); ++i) {
		lambda_env.add_exp(l_args.at(i).head(), results.at(i));
	}

	// evaluating lambda
	Expression lambda_res = lambda_e.eval(lambda_env);

	return lambda_res;
}

bool is_lambda(Environment & env, const std::string & exp) {
	return (env.is_exp(exp) && env.get_exp(exp).isHeadSymbol() && env.get_exp(exp).head().asSymbol() == "lambda");
}

Expression Expression::handle_apply_map(Environment & env) {
	// error checking
	std::string name = m_head.asSymbol();
	if (m_tail.size() == 2) {
		if (m_tail[0].isHeadSymbol() && (is_lambda(env, m_tail[0].head().asSymbol()) || env.is_proc(m_tail[0].head().asSymbol())) && m_tail[0].isTailEmpty()) {
			std::string op = m_tail[0].head().asSymbol();
			Expression evaluated = m_tail[1].eval(env);
			if (evaluated.isList()) {
				// evaluate list and get list value
				std::vector<Expression> arglist = m_tail[1].eval(env).getTail();

				// prepare args by iterating through list
				if (name == "apply") {
					// initializing expression with op as head and results as tail
					Expression a(Atom(op), arglist);
					return a.eval(env);
				}
				else {
					std::vector<Expression> results;
					std::vector<Expression> arg;
					Expression to_ret(Atom("islist"));
					for (unsigned int i = 0; i < arglist.size(); ++i) {
						arg.push_back(arglist[i]);
						Expression tmp(op, arg);
						arg.pop_back();
						results.push_back(tmp.eval(env));
					}
					to_ret.setTail(results);
					to_ret.setList();
					return to_ret;
				}
			}
			else {
				if (name == "apply")
					throw SemanticError("Error: second argument to apply not a list.");
				else
					throw SemanticError("Error: second argument to map not a list.");
			}
		}
		else {
			if (name == "apply")
				throw SemanticError("Error: first argument to apply not a procedure.");
			else
				throw SemanticError("Error: first argument to map not a procedure.");
		}
	}
	else {
		if (name == "apply")
			throw SemanticError("Error: apply takes two arguments");
		else
			throw SemanticError("Error: map takes two arguments");
	}
	return Expression();
}

Expression Expression::handle_set_property(Environment & env) {
	Expression result;
	if (m_tail.size() == 3) {
		if (m_tail[0].head().isString()) {
			std::vector<Expression> results;
			for (Expression::IteratorType it = m_tail.begin() + 1; it != m_tail.end(); ++it) {
					results.push_back(it->eval(env));
			}
			results[1].add_pair(m_tail[0], results[0]);
			result = results[1];
			if (env.is_exp(results[1].head().asSymbol())) {
				env.add_exp(results[1].head().asSymbol(), results[1]);
			}
		}
		else {
			throw SemanticError("Error in call to set-property: first argument must be a string.");
		}
	}
	else {
		throw SemanticError("Error in call to set-property: invalid number of arguments.");
	}
	return result;
}

Expression Expression::handle_get_property(Environment & env) {
	if (m_tail.size() == 2) {
		if (m_tail[0].head().isString()) {
			Expression result = m_tail[1].eval(env);
			if (m_tail[1].isHeadSymbol() && m_tail[1].isTailEmpty() && env.is_exp(m_tail[1].head().asSymbol())) {
				Expression tmp = env.get_exp(m_tail[1].head().asSymbol());
				return tmp.get_value(m_tail[0]);
			}
			else {
				return result.get_value(m_tail[0]);
			}
		}
		else {
			throw SemanticError("Error in call to get-property: first argument must be a string.");
		}
	}
	else {
		throw SemanticError("Error in call to get-property: invalid number of arguments.");
	}
	return Expression();
}

Expression make_point1(double x, double y) {
	Expression point(Atom("islist"));
	point.setList();
	std::vector<Expression> coords = { Atom(x), Atom(y) };
	point.setTail(coords);
	point.add_pair(Expression(Atom("object-name")), Expression(Atom("point")));
	point.add_pair(Expression(Atom("size")), Expression(0));
	return point;
}

Expression make_line2(Expression x, Expression y) {
	Expression line(Atom("islist"));
	line.setList();
	std::vector<Expression> coords = { x, y };
	line.setTail(coords);
	line.add_pair(Expression(Atom("object-name")), Expression(Atom("line")));
	line.add_pair(Expression(Atom("thickness")), Expression(0));
	return line;
}

bool line_split(Expression point1, Expression point2, Expression point3) {
	double ax = -(point2.getTail()[0].head().asNumber() - point1.getTail()[0].head().asNumber());
	double ay = -(point2.getTail()[1].head().asNumber() - point1.getTail()[1].head().asNumber());

	// reversing direction of other one
	double bx = point3.getTail()[0].head().asNumber() - point2.getTail()[0].head().asNumber();
	double by = point3.getTail()[1].head().asNumber() - point2.getTail()[1].head().asNumber();

	double angle = std::acos((ax * bx + ay * by) / (sqrt((ax * ax) + (ay * ay))) / (sqrt((bx * bx) + (by * by))));
	if (angle < 3.05433) {
		return true;
	}
	return false;
}

std::vector<Expression> make_box1(double minx, double maxx, double miny, double maxy) {
	std::vector<Expression> ret;
	// top left
	Expression point1 = make_point1(minx, maxy);
	// bottom left
	Expression point2 = make_point1(minx, miny);
	// top right
	Expression point3 = make_point1(maxx, maxy);
	// bottom right
	Expression point4 = make_point1(maxx, miny);

	// top center
	Expression point5 = make_point1(0, maxy);
	// bottom center
	Expression point6 = make_point1(0, miny);
	// left center
	Expression point7 = make_point1(minx, 0);
	// right center
	Expression point8 = make_point1(maxx, 0);

	// making lines
	// left top to bottom
	ret.push_back(make_line2(point1, point2));

	// top left to right
	ret.push_back(make_line2(point1, point3));

	// bottom left to right
	ret.push_back(make_line2(point2, point4));

	// right bottom to top
	ret.push_back(make_line2(point4, point3));

	// center top to bottom
	if (minx <= 0 && maxx >= 0) ret.push_back(make_line2(point5, point6));

	// center left to right
	if (miny >= 0 && maxy <= 0) ret.push_back(make_line2(point7, point8));

	return ret;
}

std::string round_pop1(double x) {
	std::stringstream precised;
	precised.precision(2);
	std::string ret;
	precised << x;
	return ret = precised.str();
}

Expression Expression::handle_continuous(Environment & env) {
	std::vector<Expression> ret;
	bool TRUE = true;
	if (m_tail.size() == 3 || m_tail.size() == 2) {
		std::vector<Expression> results;
		for (Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it) {
			results.push_back(it->eval(env));
		}
		if (results[0].head().asSymbol() == "lambda" && results[0].getTail()[0].getTail().size() == 1 && results[1].isList() && results[1].getTail().size() == 2 && results[1].getTail()[0].isHeadNumber() && results[1].getTail()[1].isHeadNumber()) {
			// getting text-scale
			double text_scale = 1;
			if (m_tail.size() == 3) {
				for (unsigned int i = 0; i < results[2].getTail().size(); ++i) {
					std::string hha = results[2].getTail()[i].getTail()[0].head().asSymbol();
					if (hha == "text-scale" && results[2].getTail()[i].getTail()[1].isHeadNumber()) {
						text_scale = results[2].getTail()[i].getTail()[1].head().asNumber();
					}
				}
			}

			// getting maxes and mins
			double maxX = results[1].getTail()[1].head().asNumber(), maxY = -100000, minX = results[1].getTail()[0].head().asNumber(), minY = 10000;
			double s_maxX, s_maxY, s_minX, s_minY;

			std::vector<Expression> x, points, lines;
			// sampling in bounds
			double low_val = results[1].getTail()[0].head().asNumber();
			double high_val = results[1].getTail()[1].head().asNumber();
			env.add_exp(Atom("continuous_lambda"), results[0]);

			std::stringstream hm;
			hm << ((high_val - low_val) / 50.0);
			hm.precision(5);
			double thing;
			hm >> thing;

			if (low_val < high_val) {
				for (double i = low_val; i <= high_val; i += thing) {
					Expression temp(Atom("continuous_lambda"));
					temp.append(i);
					double result = temp.handle_recall_lambda(env).head().asNumber();
					if (result > maxY) {
						maxY = result;
					}
					if (result < minY) {
						minY = result;
					}
				}
				Expression temp(Atom("continuous_lambda"));
				temp.append(high_val);
				double result = temp.handle_recall_lambda(env).head().asNumber();
				if (result > maxY) {
					maxY = result;
				}
				if (result < minY) {
					minY = result;
				}
			}

			double scaled_x = 20 / (maxX - minX);
			double scaled_y = 20 / (maxY - minY);

			s_maxX = (maxX * scaled_x);
			s_minX = (minX * scaled_x);
			s_maxY = (maxY * -scaled_y);
			s_minY = (minY * -scaled_y);

			if (low_val < high_val) {
				for (double i = low_val; i < high_val; i += thing) {
					// evaluate lambda
					Expression temp(Atom("continuous_lambda"));
					temp.append(i);

					// make the point
					points.push_back(make_point1(i, temp.handle_recall_lambda(env).head().asNumber()));
				}
				Expression temp(Atom("continuous_lambda"));
				temp.append(high_val);
				points.push_back(make_point1(high_val, temp.handle_recall_lambda(env).head().asNumber()));
			}

			// OU top left

			Atom hmm = Atom(round_pop1(maxY), TRUE);
			Expression OU(hmm);
			OU.add_pair(Expression(Atom("position")), make_point1(s_minX - 2, s_maxY));
			OU.add_pair(Expression(Atom("object-name")), Expression(Atom("text")));
			OU.add_pair(Expression(Atom("text-scale")), Expression(Atom(text_scale)));

			// OL bottom left left
			hmm = Atom(round_pop1(minY), TRUE);
			Expression OL(hmm);
			OL.add_pair(Expression(Atom("position")), make_point1(s_minX - 2, s_minY));
			OL.add_pair(Expression(Atom("object-name")), Expression(Atom("text")));
			OL.add_pair(Expression(Atom("text-scale")), Expression(Atom(text_scale)));

			// AL bottom left bottom
			hmm = Atom(round_pop1(minX), TRUE);
			Expression AL(hmm);
			AL.add_pair(Expression(Atom("position")), make_point1(s_minX, s_minY + 2));
			AL.add_pair(Expression(Atom("object-name")), Expression(Atom("text")));
			AL.add_pair(Expression(Atom("text-scale")), Expression(Atom(text_scale)));

			// AU 
			hmm = Atom(round_pop1(maxX), TRUE);
			Expression AU(hmm);
			AU.add_pair(Expression(Atom("position")), make_point1(s_maxX, s_minY + 2));
			AU.add_pair(Expression(Atom("object-name")), Expression(Atom("text")));
			AU.add_pair(Expression(Atom("text-scale")), Expression(Atom(text_scale)));

			ret.push_back(OL);
			ret.push_back(OU);
			ret.push_back(AL);
			ret.push_back(AU);

			std::vector<Expression> t = make_box1(s_minX, s_maxX, s_minY, s_maxY);
			for (unsigned int i = 0; i < t.size(); ++i) {
				ret.push_back(t[i]);
			}

			// iterating through options	
			Expression temp;
			if (m_tail.size() != 2) {
				for (unsigned int i = 0; i < results[2].getTail().size(); ++i) {
					if (results[2].getTail()[i].isList() && results[2].getTail()[i].getTail()[0].head().isString()) {
						std::string hha = results[2].getTail()[i].getTail()[0].head().asSymbol();
						if (hha == "title" && results[2].getTail()[i].getTail()[1].head().isString()) {
							temp = Atom(results[2].getTail()[i].getTail()[1].head().asSymbol(), TRUE);
							temp.add_pair(Expression(Atom("object-name")), Expression(Atom("text")));
							temp.add_pair(Expression(Atom("position")), make_point1(0, s_maxY - 3));
							temp.add_pair(Expression(Atom("text-scale")), Expression(Atom(text_scale)));
						}
						else if (hha == "abscissa-label" && results[2].getTail()[i].getTail()[1].head().isString()) {
							temp = Atom(results[2].getTail()[i].getTail()[1].head().asSymbol(), TRUE);
							temp.add_pair(Expression(Atom("object-name")), Expression(Atom("text")));
							temp.add_pair(Expression(Atom("position")), make_point1(0, s_minY + 3));
							temp.add_pair(Expression(Atom("text-scale")), Expression(Atom(text_scale)));
						}
						else if (hha == "ordinate-label" && results[2].getTail()[i].getTail()[1].head().isString()) {
							temp = Atom(results[2].getTail()[i].getTail()[1].head().asSymbol(), TRUE);
							temp.add_pair(Expression(Atom("object-name")), Expression(Atom("text")));
							temp.add_pair(Expression(Atom("text-rotation")), Expression(Atom(1.5708)));
							temp.add_pair(Expression(Atom("position")), make_point1(s_minX - 3, s_minY - 10));
							temp.add_pair(Expression(Atom("text-scale")), Expression(Atom(text_scale)));
						}
						else {
							break;
						}
						ret.push_back(temp);
					}
					else {
						throw SemanticError("Error in call to discrete-plot: second list must contain only string options.");
					}
				}
			}

			for (int i = 0; i < 10; ++i) {
				bool no_splits = TRUE;
				unsigned int askjlg = points.size() - 2;
				std::vector<bool> inserted(askjlg, false);
				std::vector<Expression> c_points(points.begin(), points.end());
				unsigned int insert_c = 0;
				for (unsigned int j = 0; j < points.size() - 2; ++j) {
					if (line_split(points[j], points[j + 1], points[j + 2])) {
						Expression temp(Atom("continuous_lambda"));
						temp.append((points[j + 1].getTail()[0].head().asNumber() + points[j].getTail()[0].head().asNumber()) / 2);
						Expression point1 = make_point1((points[j + 1].getTail()[0].head().asNumber() + points[j].getTail()[0].head().asNumber()) / 2, temp.handle_recall_lambda(env).head().asNumber());

						if (!inserted.at(j)) {
							c_points.insert(c_points.begin() + j + 1 + insert_c, point1);
							inserted[j] = TRUE;
							insert_c++;
						}
								
						Expression temp1(Atom("continuous_lambda"));
						temp1.append((points[j + 2].getTail()[0].head().asNumber() + points[j + 1].getTail()[0].head().asNumber()) / 2);
						Expression point2 = make_point1((points[j + 2].getTail()[0].head().asNumber() + points[j + 1].getTail()[0].head().asNumber()) / 2, temp1.handle_recall_lambda(env).head().asNumber());

						if (!inserted.at(j + 1)) {
							c_points.insert(c_points.begin() + j + 2 + insert_c, point2);
							inserted[j + 1] = TRUE;
							insert_c++;
						}
						no_splits = false;
					}
				}
				if (no_splits) {
					break;
				}
				points = c_points;
			}
			// making lines
			for (unsigned int i = 0; i < points.size(); ++i) {
				Expression thing = (make_point1(points[i].getTail()[0].head().asNumber() * scaled_x, points[i].getTail()[1].head().asNumber() * -scaled_y));
				points[i] = thing;				
			}

			for (unsigned int i = 0; i < points.size() - 1; ++i) {
				ret.push_back(make_line2(points[i], points[i + 1]));
			}
			ret.insert(ret.end(), points.begin(), points.end());
		}
		else {
			throw SemanticError("Error in call to continuous-plot: invalid arguments.");
		}
	}
	else {
		throw SemanticError("Error in call to continuous-plot: incorrect number of arguments.");
	}
	Expression to_return(Atom("islist"));
	to_return.setList();
	to_return.setTail(ret);
	return to_return;
}
// this is a simple recursive version. the iterative version is more
// difficult with the ast data structure used (no parent pointer).
// this limits the practical depth of our AST
Expression Expression::eval(Environment & env) {
	if (m_tail.empty()) {
		if (m_head.isSymbol() && m_head.asSymbol() == "list" && !m_head.isString()) {
			std::vector<Expression> results;
			return apply(m_head, results, env);
		}
		return handle_lookup(m_head, env);
	}
	// handle begin special-form
	else if (m_head.isSymbol() && m_head.asSymbol() == "begin") {
		return handle_begin(env);
	}
	// handle define special-form
	else if (m_head.isSymbol() && m_head.asSymbol() == "define") {
		return handle_define(env);
	}
	// handle apply
	else if (m_head.isSymbol() && (m_head.asSymbol() == "apply" || m_head.asSymbol() == "map")) {
		return handle_apply_map(env);
	}
	// handle lambda special-form
	else if (m_head.isSymbol() && m_head.asSymbol() == "lambda") {
		return handle_lambda();
	}
	// if lambda function is called
	else if (env.is_exp(m_head) && env.get_exp(m_head).head().asSymbol() == "lambda") {
		return handle_recall_lambda(env);
	}
	// if set-property is called
	else if (m_head.isSymbol() && m_head.asSymbol() == "set-property") {
		// handle set property
		return handle_set_property(env);
	}
	// if get-property is called
	else if (m_head.isSymbol() && m_head.asSymbol() == "get-property") {
		return handle_get_property(env);
	}
	// if continuous-plot is called
	else if (m_head.isSymbol() && m_head.asSymbol() == "continuous-plot") {
		return handle_continuous(env);
	}
	// else attempt to treat as procedure
	else {
		std::vector<Expression> results;
		for (Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it) {
			results.push_back(it->eval(env));
		}
		return apply(m_head, results, env);
	}
}

std::ostream & operator<<(std::ostream & out, const Expression & exp) {
	if (exp.head().isNone() && exp.isTailEmpty()) {
		out << "NONE";
	}
	else {
		static bool is_lambda = false;
		out << "(";
		if (exp.head().asSymbol() == "lambda") {
			is_lambda = true;
			if (!exp.isTailEmpty()) {
				out << *exp.tailConstBegin();
				if (exp.getTail().size() > 1) {
					out << " ";
					out << *(exp.tailConstBegin() + 1);
				} 
			}
			is_lambda = false;
		}
		else {
			if (!exp.isList()) {
				out << exp.head();
			}
			if (is_lambda && !exp.isList() && !(exp.isTailEmpty())) {
				out << " ";
			}
			for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e) {
				out << *e;
				if ((is_lambda || exp.isList()) && e != (exp.tailConstEnd() - 1)) {
					out << " ";
				}
			}
		}
		out << ")";
	}
	return out;
}

bool Expression::operator==(const Expression & exp) const noexcept {

	bool result = (m_head == exp.m_head);

	result = result && (m_tail.size() == exp.m_tail.size());

	if (result) {
		for (auto lefte = m_tail.begin(), righte = exp.m_tail.begin();
			(lefte != m_tail.end()) && (righte != exp.m_tail.end());
			++lefte, ++righte) {
			result = result && (*lefte == *righte);
		}
	}

	return result;
}

void Expression::add_pair(const Expression & key, const Expression & value) {
	if (m_properties.find(key.head().asSymbol()) != m_properties.end()) {
		m_properties.at(key.head().asSymbol()) = value;
	}
	else {
		m_properties.insert(std::make_pair(key.head().asSymbol(), value));
	}
}

Expression Expression::get_value(Expression & key) {
	if (m_properties.find(key.head().asSymbol()) == m_properties.end()) {
		return Expression();
	}
	return m_properties.at(key.head().asSymbol());
}

bool Expression::is_value(Expression & key) {
	return (m_properties.find(key.head().asSymbol()) != m_properties.end());
}

void Expression::setList()
{
	is_list = true;
}

bool Expression::isList() const noexcept
{
	return is_list;
}

bool operator!=(const Expression & left, const Expression & right) noexcept {

	return !(left == right);
}