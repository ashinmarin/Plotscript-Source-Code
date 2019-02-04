#include "environment.hpp"

#include <cassert>
#include <cmath>
#include <math.h>
#include <sstream>

#include "environment.hpp"
#include "semantic_error.hpp"

/***********************************************************************
Helper Functions
**********************************************************************/

// predicate, the number of args is nargs
bool nargs_equal(const std::vector<Expression> & args, unsigned nargs) {
	return args.size() == nargs;
}
/***********************************************************************
Each of the functions below have the signature that corresponds to the
typedef'd Procedure function pointer.
**********************************************************************/

// the default procedure always returns an expresison of type None
Expression default_proc(const std::vector<Expression> & args) {
	args.size(); // make compiler happy we used this parameter
	return Expression();
};

Expression add(const std::vector<Expression> & args) {
	// check all aruments are numbers, while adding
	double result = 0;
	std::complex<double> cresult(0.0, 0.0);
	bool complexRes = false;

	for (auto & a : args) {
		if (a.isHeadNumber() && !complexRes) {
			result += a.head().asNumber();
		}
		else if (a.isHeadNumber()) {
			cresult += a.head().asNumber();
		}
		else if (a.isHeadComplex()) {
			cresult += result;
			cresult += a.head().asComplex();
			complexRes = true;
		}
		else {
			throw SemanticError("Error in call to add, argument not a number");
		}
	}
	if (complexRes) {
		return Expression(cresult);
	}
	return Expression(result);
};

Expression mul(const std::vector<Expression> & args) {
	// check all aruments are numbers, while multiplying
	double result = 1;
	std::complex<double> cresult(1.0, 0.0);
	bool complexRes = false;

	for (auto & a : args) {
		if (a.isHeadNumber() && !complexRes) {
			result *= a.head().asNumber();
		}
		else if (a.isHeadNumber()) {
			cresult *= a.head().asNumber();
		}
		else if (a.isHeadComplex()) {
			if (!complexRes) {
				cresult *= result;
			}
			cresult *= a.head().asComplex();
			complexRes = true;
		}
		else {
			throw SemanticError("Error in call to mul, argument not a number");
		}
	}
	if (complexRes) {
		return Expression(cresult);
	}
	return Expression(result);
};

Expression subneg(const std::vector<Expression> & args) {
	double result = 0;
	std::complex<double> cresult(0.0, 0.0);
	bool complexRes = true;

	// preconditions
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {
			result = -args[0].head().asNumber();
			complexRes = false;
		}
		else if (args[0].isHeadComplex()) {
			cresult = -args[0].head().asComplex();
		}
		else {
			throw SemanticError("Error in call to negate: invalid argument.");
		}
	}
	else if (nargs_equal(args, 2)) {
		if ((args[0].isHeadNumber()) && (args[1].isHeadNumber())) {
			result = args[0].head().asNumber() - args[1].head().asNumber();
			complexRes = false;
		}
		else if ((args[0].isHeadComplex()) && (args[1].isHeadNumber())) {
			cresult = args[0].head().asComplex() - args[1].head().asNumber();
		}
		else if ((args[0].isHeadNumber()) && (args[1].isHeadComplex())) {
			cresult = args[0].head().asNumber() - args[1].head().asComplex();
		}
		else if ((args[0].isHeadComplex()) && (args[1].isHeadComplex())) {
			cresult = args[0].head().asComplex() - args[1].head().asComplex();
		}
	}
	else {
		throw SemanticError("Error in call to subtraction or negation: invalid number of arguments.");
	}
	if (complexRes) {
		return Expression(cresult);
	}
	return Expression(result);
};

Expression div(const std::vector<Expression> & args) {
	double result = 0;
	std::complex<double> cresult(1.0, 0.0);
	bool complexRes = true;
	if (nargs_equal(args, 1)) {
		if ((args[0].isHeadNumber())) {
			result = 1 / args[0].head().asNumber();
			complexRes = false;
		}
		else if ((args[0].isHeadComplex())) {
			cresult = cresult / args[0].head().asComplex();
		}
		else {
			throw SemanticError("Error in call to division: argument not a number.");
		}
	}
	else if (nargs_equal(args, 2)) {
		if ((args[0].isHeadNumber()) && (args[1].isHeadNumber())) {
			result = args[0].head().asNumber() / args[1].head().asNumber();
			complexRes = false;
		}
		else if ((args[0].isHeadComplex()) && args[1].isHeadNumber()) {
			cresult = args[0].head().asComplex() / args[1].head().asNumber();
		}
		else if ((args[0].isHeadNumber()) && (args[1].isHeadComplex())) {
			cresult = args[0].head().asNumber() / args[1].head().asComplex();
		}
		else if ((args[0].isHeadComplex() && args[1].isHeadComplex())) {
			cresult = args[0].head().asComplex() / args[1].head().asComplex();
		}
	}
	else {
		throw SemanticError("Error in call to division: invalid number of arguments.");
	}
	if (complexRes) {
		return Expression(cresult);
	}
	return Expression(result);
};

Expression sqroot(const std::vector<Expression> & args) {
	double result = 0;
	std::complex<double> cresult(0.0, 0.0);
	bool complexRes = false;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			cresult = std::sqrt(args[0].head().asComplex());
			complexRes = true;
		}
		else if (args[0].isHeadNumber()) {
			if (args[0].head().asNumber() > -1) {
				result = std::sqrt(args[0].head().asNumber());
			}
			else if (args[0].head().asNumber() < 0) {
				std::complex<double> ctemp(args[0].head().asNumber(), 0.0);
				cresult = std::sqrt(ctemp);
				complexRes = true;
			}
		}
		else {
			throw SemanticError("Error in call to square root: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to square root: invalid number of arguments.");
	}
	if (complexRes) {
		return Expression(cresult);
	}
	return Expression(result);
}

Expression expo(const std::vector<Expression> & args) {
	double result = 0;
	std::complex<double> cresult(0.0, 0.0);
	bool complexRes = true;

	if (nargs_equal(args, 2)) {
		if (args[0].isHeadComplex() && args[1].isHeadComplex()) {
			cresult = std::pow(args[0].head().asComplex(), args[1].head().asComplex());
		}
		else if (args[0].isHeadComplex() && args[1].isHeadNumber()) {
			cresult = std::pow(args[0].head().asComplex(), args[1].head().asNumber());
		}
		else if (args[0].isHeadNumber() && args[1].isHeadComplex()) {
			cresult = std::pow(args[0].head().asNumber(), args[1].head().asComplex());
		}
		else if (args[0].isHeadNumber() && args[1].isHeadNumber()) {
			result = std::pow(args[0].head().asNumber(), args[1].head().asNumber());
			complexRes = false;
		}
	}
	else {
		throw SemanticError("Error in call to exponential: invalid number of arguments.");
	}
	if (complexRes) {
		return Expression(cresult);
	}
	return Expression(result);
}

//ln 
Expression nln(const std::vector<Expression> & args) {
	double result = 0;
	std::complex<double> cresult(0.0, 0.0);
	bool complexRes = false;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			cresult = std::log(args[0].head().asComplex());
			complexRes = true;
		}
		else if (args[0].isHeadNumber()) {
			if (args[0].head().asNumber() > 0) {
				result = std::log(args[0].head().asNumber());
			}
			else if (args[0].head().asNumber() < 0) {
				std::complex<double> tmp(args[0].head().asNumber(), 0.0);
				cresult = std::log(tmp);
				complexRes = true;
			}
		}
	}
	else {
		throw SemanticError("Error in call to ln: invalid number of arguments.");
	}
	if (complexRes) {
		return Expression(cresult);
	}
	return Expression(result);
}

Expression sine(const std::vector<Expression> & args) {
	double result = 0;
	std::complex<double> cresult(0.0, 0.0);
	bool complexRes = false;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			complexRes = true;
			cresult = std::sin(args[0].head().asComplex());
		}
		else if (args[0].isHeadNumber()) {
			result = std::sin(args[0].head().asNumber());
		}
		else {
			throw SemanticError("Error in call to sin: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to sin: invalid number of arguments.");
	}
	if (complexRes) {
		return Expression(cresult);
	}
	return Expression(result);
}

Expression cosine(const std::vector<Expression> & args) {
	double result = 0;
	std::complex<double> cresult(0.0, 0.0);
	bool complexRes = false;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			complexRes = true;
			cresult = std::cos(args[0].head().asComplex());
		}
		else if (args[0].isHeadNumber()) {
			result = std::cos(args[0].head().asNumber());
		}
		else {
			throw SemanticError("Error in call to cos: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to cos: invalid number of arguments.");
	}
	if (complexRes) {
		return Expression(cresult);
	}
	return Expression(result);
}

Expression tangent(const std::vector<Expression> & args) {
	double result = 0;
	std::complex<double> cresult(0.0, 0.0);
	bool complexRes = false;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			complexRes = true;
			cresult = std::tan(args[0].head().asComplex());
		}
		else if (args[0].isHeadNumber()) {
			result = std::tan(args[0].head().asNumber());
		}
		else {
			throw SemanticError("Error in call to tan: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to tan: invalid number of arguments.");
	}
	if (complexRes) {
		return Expression(cresult);
	}
	return Expression(result);
}

Expression complex_real(const std::vector<Expression> & args) {
	double result = 0;
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = args[0].head().ComplexReal();
		}
		else {
			throw SemanticError("Error in call to real: argument must be complex.");
		}
	}
	else {
		throw SemanticError("Error in call to real: invalid number of arguments.");
	}
	return Expression(result);
}

Expression complex_imag(const std::vector<Expression> & args) {
	double result = 0;
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = args[0].head().ComplexImag();
		}
		else {
			throw SemanticError("Error in call to imag: argument must be complex.");
		}
	}
	else {
		throw SemanticError("Error in call to imag: invalid number of arguments.");
	}
	return Expression(result);
}

Expression complex_mag(const std::vector<Expression> & args) {
	double result = 0;
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = args[0].head().ComplexMagnitude();
		}
		else {
			throw SemanticError("Error in call to mag: argument must be complex.");
		}
	}
	else {
		throw SemanticError("Error in call to mag: invalid number of arguments.");
	}
	return Expression(result);
}

Expression complex_arg(const std::vector<Expression> & args) {
	double result = 0;
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = args[0].head().ComplexArgument();
		}
		else {
			throw SemanticError("Error in call to arg: argument must be complex.");
		}
	}
	else {
		throw SemanticError("Error in call to arg: invalid number of arguments.");
	}
	return Expression(result);
}

Expression complex_conj(const std::vector<Expression> & args) {
	std::complex<double> result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = args[0].head().ComplexConjugate();
		}
		else {
			throw SemanticError("Error in call to conj: argument must be complex.");
		}
	}
	else {
		throw SemanticError("Error in call to conj: invalid number of arguments.");
	}
	return Expression(result);
}

Expression lists(const std::vector<Expression> & args) {
	Expression result(Atom("islist"));
	result.setList();
	if (nargs_equal(args, 0)) {
		return result;
	}
	else {		
		result.setTail(args);
	}
	return result;
}

Expression first(const std::vector<Expression> & args) {
	Expression result;
	if (nargs_equal(args, 1)) {
		if (args[0].isList()) {
			if (args[0].getTail().size() > 0) {
				result = (args[0].getTail().at(0));
			}
			else {
				throw SemanticError("Error in call to first: list cannot be empty.");
			}
		}
		else {
			throw SemanticError("Error in call to first: argument not a list.");
		}
	}
	else {
		throw SemanticError("Error in call to first: invalid number of arguments.");
	}
	return result;
}

Expression rest(const std::vector<Expression> & args) {
	Expression result(Atom("islist"));
	if (nargs_equal(args, 1)) {
		if (args[0].isList()) {
			if (args[0].getTail().size() > 0) {
				std::vector<Expression> tmp = args[0].getTail();
				tmp.erase(tmp.begin());
				result.setTail(tmp);
				result.setList();
			}
			else {
				throw SemanticError("Error in call to rest: list cannot be empty.");
			}
		}
		else {
			throw SemanticError("Error in call to rest: argument not a list.");
		}
	}
	else {
		throw SemanticError("Error in call to rest: invalid number of arguments.");
	}
	return Expression(result);	
}

Expression length(const std::vector<Expression> & args) {
	std::size_t result;
	if (nargs_equal(args, 1)) {
		if (args[0].isList()) {
				result = args[0].getTail().size();
		}
		else {
			throw SemanticError("Error in call to length: argument not a list.");
		}
	}
	else {
		throw SemanticError("Error in call to length: invalid number of arguments.");
	}
	return Expression(result);
}

Expression append(const std::vector<Expression> & args) {
	Expression result(Atom("islist"));
	if (nargs_equal(args, 2)) {
		if (args[0].isList()) {
			std::vector<Expression> tmp = args[0].getTail();
			tmp.emplace_back(args[1]);
			result.setTail(tmp);
			result.setList();
		}
		else {
			throw SemanticError("Error in call to append: first argument is not a list.");
		}
	}
	else {
		throw SemanticError("Error in call to append: invalid number of arguments.");
	}
	return result;
}

Expression join(const std::vector<Expression> & args) {
	Expression result(Atom("islist"));
	if (nargs_equal(args, 2)) {
		if (args[0].isList() && args[1].isList()) {
			std::vector<Expression> tmp = args[0].getTail();
			std::vector<Expression> tmp1 = args[1].getTail();
			tmp.insert(tmp.end(), tmp1.begin(), tmp1.end());

			result.setTail(tmp);
			result.setList();
		}
		else {
			throw SemanticError("Error in call to join: argument to join is not a list.");
		}
	}
	else {
		throw SemanticError("Error in call to join: invalid number of arguments.");
	}
	return Expression(result);
}

Expression range(const std::vector<Expression> & args) {
	std::vector<Expression> tmp;
	Expression result(Atom("islist"));
	if (nargs_equal(args, 3)) {
		if (args[0].isHeadNumber() && args[1].isHeadNumber() && args[2].isHeadNumber()) {
			if (args[0].head().asNumber() < args[1].head().asNumber()) {
				if (args[2].head().asNumber() > 0) {
					for (double i = args[0].head().asNumber(); i <= args[1].head().asNumber(); i += args[2].head().asNumber()) {
						tmp.emplace_back(Atom(i));
					}
					result.setTail(tmp);
					result.setList();
				}
				else {
					throw SemanticError("Error in call to range: third argument must be positive");
				}
			}
			else {
				throw SemanticError("Error in call to range: first argument must be less than second argument.");
			}
		}
		else {
			throw SemanticError("Error in call to range: arguments must be numbers.");
		}
	}
	else {
		throw SemanticError("Error in call to range: invalid number of arguments.");
	}
	return result;
}

Expression make_point(double x, double y) {
	Expression point(Atom("islist"));
	point.setList();
	std::vector<Expression> coords = { Atom(x), Atom(y) };
	point.setTail(coords);
	point.add_pair(Expression(Atom("object-name")), Expression(Atom("point")));
	return point;
}

Expression make_line(Expression x, Expression y) {
	Expression line(Atom("islist"));
	line.setList();
	std::vector<Expression> coords = { x, y };
	line.setTail(coords);
	line.add_pair(Expression(Atom("object-name")), Expression(Atom("line")));
	return line;
}

// returns a list of six lines
/*
-10,-10 to -10,10
-10,-10 to 10,-10
-10,10 to 10,10
10,10 to 10,-10
0,-10 to 0,10
-10,0 to 10,0
*/
std::vector<Expression> make_box(double minx, double maxx, double miny, double maxy) {
	std::vector<Expression> ret;
	// top left
	Expression point1 = make_point(minx, maxy);
	// bottom left
	Expression point2 = make_point(minx, miny);
	// top right
	Expression point3 = make_point(maxx, maxy);
	// bottom right
	Expression point4 = make_point(maxx, miny);

	
	// top center
	Expression point5 = make_point(0, maxy);
	// bottom center
	Expression point6 = make_point(0, miny);
	// left center
	Expression point7 = make_point(minx, 0);
	// right center
	Expression point8 = make_point(maxx, 0);

	// making lines
	// left top to bottom
	ret.push_back(make_line(point1, point2));

	// top left to right
	ret.push_back(make_line(point1, point3));

	// bottom left to right
	ret.push_back(make_line(point2, point4));

	// right bottom to top
	ret.push_back(make_line(point4, point3));

	// center top to bottom
	if (minx <= 0 && maxx >= 0) ret.push_back(make_line(point5, point6));

	// center left to right
	if (miny >= 0 && maxy <= 0) ret.push_back(make_line(point7, point8));

	return ret;
}

std::string round_pop(double x) {
	std::stringstream precised;
	precised.precision(2);
	std::string ret;
	precised << x;
	return ret = precised.str();
}

Expression discrete_plot(const std::vector<Expression> & args) {
	Expression result(Atom("islist"));
	result.setList();
	std::vector<Expression> tail;

	if (nargs_equal(args, 2)) {
		if (args[0].isList() && args[1].isList()) {
			// getting text-scale
			double t_scale = 1;
			for (unsigned int i = 0; i < args[1].getTail().size(); ++i) {
				std::string text_scale = args[1].getTail()[i].getTail()[0].head().asSymbol();
				if (text_scale == "text-scale" && args[1].getTail()[i].getTail()[1].isHeadNumber()) {
					t_scale = args[1].getTail()[i].getTail()[1].head().asNumber();
				}
			}
			// getting maxes and mins
			float maxX = -10000, maxY = -100000, minX = 10000, minY = 10000;
			float s_maxX, s_maxY, s_minX, s_minY;

			// getting data, adding lollipops
			for (unsigned int i = 0; i < args[0].getTail().size(); ++i) {
				Expression temp = args[0].getTail()[i];
				// checking x
				float x = (float)temp.getTail()[0].head().asNumber();
				if (x > maxX) maxX = x;
				if (x < minX) minX = x;

				// checking y
				float y = (float)temp.getTail()[1].head().asNumber();
				if (y > maxY) maxY = y;
				if (y < minY) minY = y;
			}

			double scaled_x = 20 / (maxX - minX);
			double scaled_y = 20 / (maxY - minY);

			for (unsigned int i = 0; i < args[0].getTail().size(); ++i) {
				if (args[0].getTail()[i].isList() && args[0].getTail()[i].getTail().size() == 2) {

					// updating for bounding box
					s_maxX = (float)(maxX * scaled_x);
					s_minX = (float)(minX * scaled_x);
					s_maxY = (float)(maxY * -scaled_y);
					s_minY = (float)(minY * -scaled_y);

					Expression temp(Atom("islist"));
					temp.setList();

					// scaling and pushing
					std::vector<Expression> sap;
					sap.push_back(Expression(args[0].getTail()[i].getTail()[0].head().asNumber() * scaled_x));
					sap.push_back(Expression(args[0].getTail()[i].getTail()[1].head().asNumber() * -scaled_y));

					temp.setTail(sap);
					temp.add_pair(Expression(Atom("object-name")), Expression(Atom("point")));
					temp.add_pair(Expression(Atom("size")), Expression(Atom(0.5)));
					tail.push_back(temp);
					
					// adding a line for every point
					Expression zeroed = make_point(temp.getTail()[0].head().asNumber(), (0 < s_minY) ? 0: s_minY);
					Expression lollypop_line = make_line(zeroed, temp);
					tail.push_back(lollypop_line);

				}
				else {
					throw SemanticError("Error in call to discrete plot: first list must consist of coordinates.");
				}
			}

			// AL, AU, OL, OU

			// OU top left
			Atom hm = Atom(round_pop(maxY), true);
			Expression OU(hm);
			OU.add_pair(Expression(Atom("position")), make_point(s_minX - 2, s_maxY));
			OU.add_pair(Expression(Atom("object-name")), Expression(Atom("text")));
			OU.add_pair(Expression(Atom("text-scale")), Expression(Atom(t_scale)));

			// OL bottom left left
			hm = Atom(round_pop(minY), true);
			Expression OL(hm);
			OL.add_pair(Expression(Atom("position")), make_point(s_minX - 2, s_minY));
			OL.add_pair(Expression(Atom("object-name")), Expression(Atom("text")));
			OL.add_pair(Expression(Atom("text-scale")), Expression(Atom(t_scale)));

			// AL bottom left bottom
			hm = Atom(round_pop(minX), true);
			Expression AL(hm);
			AL.add_pair(Expression(Atom("position")), make_point(s_minX, s_minY + 2));
			AL.add_pair(Expression(Atom("object-name")), Expression(Atom("text")));
			AL.add_pair(Expression(Atom("text-scale")), Expression(Atom(t_scale)));

			// AU 
			hm = Atom(round_pop(maxX), true);
			Expression AU(hm);
			AU.add_pair(Expression(Atom("position")), make_point(s_maxX, s_minY + 2));
			AU.add_pair(Expression(Atom("object-name")), Expression(Atom("text")));
			AU.add_pair(Expression(Atom("text-scale")), Expression(Atom(t_scale)));

			tail.push_back(OU);
			tail.push_back(OL);
			tail.push_back(AL);
			tail.push_back(AU);

			// iterating through options	
			Expression temp;
			for (unsigned int i = 0; i < args[1].getTail().size(); ++i) {
				if (args[1].getTail()[i].isList() && args[1].getTail()[i].getTail()[0].head().isString()) {
					std::string hha = args[1].getTail()[i].getTail()[0].head().asSymbol();
					if (hha == "title" && args[1].getTail()[i].getTail()[1].head().isString()) {
						temp = Atom(args[1].getTail()[i].getTail()[1].head().asSymbol(), true);
						temp.add_pair(Expression(Atom("object-name")), Expression(Atom("text")));
						temp.add_pair(Expression(Atom("position")), make_point(0, s_maxY - 3));
						temp.add_pair(Expression(Atom("text-scale")), Expression(Atom(t_scale)));
					}
					else if (hha == "abscissa-label" && args[1].getTail()[i].getTail()[1].head().isString()) {
						temp = Atom(args[1].getTail()[i].getTail()[1].head().asSymbol(), true);
						temp.add_pair(Expression(Atom("object-name")), Expression(Atom("text")));
						temp.add_pair(Expression(Atom("position")), make_point(0, s_minY + 3));
						temp.add_pair(Expression(Atom("text-scale")), Expression(Atom(t_scale)));
					}
					else if (hha == "ordinate-label" && args[1].getTail()[i].getTail()[1].head().isString()) {
						temp = Atom(args[1].getTail()[i].getTail()[1].head().asSymbol(), true);
						temp.add_pair(Expression(Atom("object-name")), Expression(Atom("text")));
						temp.add_pair(Expression(Atom("text-rotation")), Expression(Atom(1.5708)));
						temp.add_pair(Expression(Atom("position")), make_point(s_minX - 3, s_minY - 10));
						temp.add_pair(Expression(Atom("text-scale")), Expression(Atom(t_scale)));
					}
					else {
						break;
					}
					tail.push_back(temp);
				}
				else {
					throw SemanticError("Error in call to discrete-plot: second list must contain only string options.");
				}
			}

			// adding bounding box
			std::vector<Expression> t = make_box(s_minX, s_maxX, s_minY, s_maxY);
			for (unsigned int i = 0; i < t.size(); ++i) {
				tail.push_back(t[i]);
			}
		}
		else {
			throw SemanticError("Error in call to discrete plot: both arguments must be lists.");
		}
	}
	else {
		throw SemanticError("Error in call to discrete plot: invalid number of arguments.");
	}
	result.setTail(tail);
	return result;
}

/*std::vector<Expression> results;
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

	return lambda_res;*/

const double PI = std::atan2(0, -1);
const double EXP = std::exp(1);
const std::complex<double> I(0.0, 1.0);

Environment::Environment() {

	reset();
}

Environment::Environment(const Environment & env) {
	std::map<std::string, EnvResult> tmp(env.envmap);
	envmap = tmp;
}

bool Environment::is_known(const Atom & sym) const {
	if (!sym.isSymbol()) return false;

	return envmap.find(sym.asSymbol()) != envmap.end();
}

bool Environment::is_exp(const Atom & sym) const {
	if (!sym.isSymbol()) return false;

	auto result = envmap.find(sym.asSymbol());
	return (result != envmap.end()) && (result->second.type == ExpressionType);
}

Expression Environment::get_exp(const Atom & sym) const {

	Expression exp;

	if (sym.isSymbol()) {
		auto result = envmap.find(sym.asSymbol());
		if ((result != envmap.end()) && (result->second.type == ExpressionType)) {
			exp = result->second.exp;
		}
	}

	return exp;
}

void Environment::add_exp(const Atom & sym, const Expression & exp) {

	if (!sym.isSymbol()) {
		throw SemanticError("Attempt to add non-symbol to environment");
	}

	// error if overwriting symbol map
	if (envmap.find(sym.asSymbol()) != envmap.end()) {
		envmap[sym.asSymbol()] = EnvResult(ExpressionType, exp);
	}

	envmap.emplace(sym.asSymbol(), EnvResult(ExpressionType, exp));
}

bool Environment::is_proc(const Atom & sym) const {
	if (!sym.isSymbol()) return false;

	auto result = envmap.find(sym.asSymbol());
	return (result != envmap.end()) && (result->second.type == ProcedureType);
}

Procedure Environment::get_proc(const Atom & sym) const {

	if (sym.isSymbol()) {
		auto result = envmap.find(sym.asSymbol());
		if ((result != envmap.end()) && (result->second.type == ProcedureType)) {
			return result->second.proc;
		}
	}

	return default_proc;
}

/*
Reset the environment to the default state. First remove all entries and
then re-add the default ones.
*/
void Environment::reset() {

	envmap.clear();

	// Built-In value of pi
	envmap.emplace("pi", EnvResult(ExpressionType, Expression(PI)));

	// built-in value of exp
	envmap.emplace("e", EnvResult(ExpressionType, Expression(EXP)));

	// built-in value of i
	envmap.emplace("I", EnvResult(ExpressionType, Expression(I)));

	// Procedure: add;
	envmap.emplace("+", EnvResult(ProcedureType, add));

	// Procedure: subneg;
	envmap.emplace("-", EnvResult(ProcedureType, subneg));

	// Procedure: mul;
	envmap.emplace("*", EnvResult(ProcedureType, mul));

	// Procedure: div;
	envmap.emplace("/", EnvResult(ProcedureType, div));

	// Procedure: sqrt;
	envmap.emplace("sqrt", EnvResult(ProcedureType, sqroot));

	// Procedure: exponential;
	envmap.emplace("^", EnvResult(ProcedureType, expo));

	// Procedure: ln;
	envmap.emplace("ln", EnvResult(ProcedureType, nln));

	// Procedure: sin;
	envmap.emplace("sin", EnvResult(ProcedureType, sine));

	// Procedure: cos;
	envmap.emplace("cos", EnvResult(ProcedureType, cosine));

	// Procedure: tan;
	envmap.emplace("tan", EnvResult(ProcedureType, tangent));

	// Procedure: real;
	envmap.emplace("real", EnvResult(ProcedureType, complex_real));

	// Procedure: imag;
	envmap.emplace("imag", EnvResult(ProcedureType, complex_imag));

	// Procedure: mag;
	envmap.emplace("mag", EnvResult(ProcedureType, complex_mag));

	// Procedure: arg;
	envmap.emplace("arg", EnvResult(ProcedureType, complex_arg));

	// Procedure: conj;
	envmap.emplace("conj", EnvResult(ProcedureType, complex_conj));

	// Procedure: list
	envmap.emplace("list", EnvResult(ProcedureType, lists));

	// Procedure: first
	envmap.emplace("first", EnvResult(ProcedureType, first));

	// Procedure: rest
	envmap.emplace("rest", EnvResult(ProcedureType, rest));

	// Procedure: length
	envmap.emplace("length", EnvResult(ProcedureType, length));

	// Procedure: append
	envmap.emplace("append", EnvResult(ProcedureType, append));

	// Procedure: join
	envmap.emplace("join", EnvResult(ProcedureType, join));

	// Procedure: range
	envmap.emplace("range", EnvResult(ProcedureType, range));

	// Procedure: discrete-plot
	envmap.emplace("discrete-plot", EnvResult(ProcedureType, discrete_plot));
}