/*! \file parse.hpp
Defines the parse function.
 */
#ifndef PARSE_HPP
#define PARSE_HPP

#include "token.hpp"
#include "expression.hpp"

/*! \fn parse
\brief parse a sequence of tokens into an expression (abstract syntax tree)

\param tokens, the input token sequence
\returns the expression resulting from parsing or the None Expression on failure
 */
Expression parse(const TokenSequenceType & tokens) noexcept;

#endif
