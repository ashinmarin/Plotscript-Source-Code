/*! \file expression.hpp
Defines the Expression type and assiciated functions.
 */
#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "token.hpp"
#include "atom.hpp"

 // forward declare Environment
class Environment;

/*! \class Expression
\brief An expression is a tree of Atoms.

An expression is an atom called the head followed by a (possibly empty)
list of expressions called the tail.
 */

class Expression {
public:

	typedef std::vector<Expression>::const_iterator ConstIteratorType;

	/// Default construct and Expression, whose type in NoneType
	Expression();

	/*! Construct an Expression with given Atom as head an empty tail
	  \param atom the atom to make the head
	*/
	Expression(const Atom & a);

	/// lambda constructor
	Expression(const Expression & a, const Expression & b);

	/// head and tail constructor
	Expression(const Atom & a, const std::vector<Expression> & b) : m_head(a), m_tail(b) {};

	/// deep-copy construct an expression (recursive)
	Expression(const Expression & a);

	/// deep-copy assign an expression  (recursive)
	Expression & operator=(const Expression & a);

	/// return a reference to the head Atom
	Atom & head();

	/// return a const-reference to the head Atom
	const Atom & head() const;

	bool isHeadComplex() const noexcept;

	/// append Atom to tail of the expression
	void append(const Atom & a);

	/// append Expression to tail of the expression
	void append(const Expression & exp);

	/// return a pointer to the last expression in the tail, or nullptr
	Expression * tail();

	/// returns whether tail is empty
	bool isTailEmpty() const noexcept;

	/// sets tail
	void setTail(std::vector<Expression> to_add);

	/// gets tail
	std::vector<Expression> getTail() const noexcept;

	/// return a const-iterator to the beginning of tail
	ConstIteratorType tailConstBegin() const noexcept;

	/// return a const-iterator to the tail end
	ConstIteratorType tailConstEnd() const noexcept;

	/// convienience member to determine if head atom is a number
	bool isHeadNumber() const noexcept;

	/// convienience member to determine if head atom is a symbol
	bool isHeadSymbol() const noexcept;

	/// Evaluate expression using a post-order traversal (recursive)
	Expression eval(Environment & env);

	/// equality comparison for two expressions (recursive)
	bool operator==(const Expression & exp) const noexcept;

	void add_pair(const Expression & key, const Expression & value);
	Expression get_value(Expression & key);
	bool is_value(Expression & key);
	void setList();

	bool isList() const noexcept;

	Expression handle_recall_lambda(Environment & env);
private:

	// the head of the expression
	Atom m_head;

	// the tail list is expressed as a vector for access efficiency
	// and cache coherence, at the cost of wasted memory.
	std::vector<Expression> m_tail;

	// convenience typedef
	typedef std::vector<Expression>::iterator IteratorType;

	// property map
	std::map<std::string, Expression> m_properties;

	// list
	bool is_list = false;

	// internal helper methods
	Expression handle_lookup(const Atom & head, const Environment & env);
	Expression handle_define(Environment & env);
	Expression handle_lambda();
	std::vector<Expression> exp_to_atom(Expression e);
	
	Expression handle_begin(Environment & env);
	Expression handle_apply_map(Environment & env);
	Expression handle_set_property(Environment & env);
	Expression handle_get_property(Environment & env);
	Expression handle_continuous(Environment & env);
	Expression continuous_lambda(Environment & env, Expression func);
};

/// Render expression to output stream
std::ostream & operator<<(std::ostream & out, const Expression & exp);

/// inequality comparison for two expressions (recursive)
bool operator!=(const Expression & left, const Expression & right) noexcept;

#endif