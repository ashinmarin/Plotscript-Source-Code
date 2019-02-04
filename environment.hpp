/*! \file environment.hpp
This file defines the module representing the environment.

The environment is the collection of all previously defined procedures, e.g.
the built-in ones, and previously defined symbols. The environment is used
by the interpreter to store and lookup the value of symbols and procedures as
needed.
 */

#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

 // system includes
#include <map>

// module includes
#include "atom.hpp"
#include "expression.hpp"

/*! \typedef Procedure
\brief A Procedure is a C++ function pointer taking a vector of
	   Expressions as arguments and returning an Expression.
*/
typedef Expression(*Procedure)(const std::vector<Expression> & args);

/*! \class Environment
\brief A class representing the interpreter environment.

An instance of Environment allows the interpreter to track previously defined
procedures and definitions, either built-in or defined during execution.

To lookup a symbol use one of the member functions is_exp or is_proc, or if you
do not care about
what the symbol maps to is_known. Depending on the value these member functions
return you can obtain
the mapped-to value using get_exp or get_proc.

To add an symbol to expression mapping use the add_exp member function.
 */
class Environment {
public:
	/*! Construct the default environment with built-in procedures and
	 * definitions. */
	Environment();

	/*! Copy constructor for Environment */
	Environment(const Environment & env);

	/*! Determine if a symbol is known to the environment.
	  \param sym the sumbol to lookup
	  \return true if the symbol has been defined in the environment
	 */
	bool is_known(const Atom &sym) const;

	/*! Determine if a symbol has been defined as an expression.
	  \param sym the sumbol to lookup
	  \return true if the symbol has been defined in the environment as an
	  expression
	 */
	bool is_exp(const Atom &sym) const;

	/*! Get the Expression the argument symbol maps to.
	  \param sym the symbol to lookup
	  \return the expression the symbol maps to or an Expression of NoneType
	*/
	Expression get_exp(const Atom &sym) const;

	/*! Add a mapping from sym argument to the exp argument within the environment.
	  \param sym the symbol to add
	  \param exp the expression the symbol should map to
	 */
	void add_exp(const Atom &sym, const Expression &exp);

	/*! Determine if a symbol has been defined as a procedure
	  \param sym the symbol to lookup
	  \return true if thr symbol maps to a procedure
	 */
	bool is_proc(const Atom &sym) const;

	/*! Get the Procedure the argument symbol maps to
	  \param sym the symbol to lookup
	  \return the procedure it maps to

	  Note: return the default procedure if argument is not a symbol
			or does not map to a known procedure.
	*/
	Procedure get_proc(const Atom &sym) const;

	/*! Reset the environment to its default state. */
	void reset();

	private:
	bool is_lambda = false;
	// Environment is a mapping from symbols to expressions or procedures
	enum EnvResultType { ExpressionType, ProcedureType };

	struct EnvResult {
		EnvResultType type;
		Expression exp; // used when type is ExpressionType
		Procedure proc; // used when type is ProcedureType

		// constructors for use in container emplace
		EnvResult() {};
		EnvResult(EnvResultType t, Expression e) : type(t), exp(e) {};
		EnvResult(EnvResultType t, Procedure p) : type(t), proc(p) {};
	};

	// the environment map
	std::map<std::string, EnvResult> envmap;
};

#endif
