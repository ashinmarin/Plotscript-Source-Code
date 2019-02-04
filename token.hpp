/*! \file token.hpp
Defines the Token and TokenSequence types, and associated functions.
*/
#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <complex>
#include <deque>
#include <istream>
#include <vector>

/*! \class Token
\brief Value class representing a token.

A token is a composition of a tag type and an optional string value.
*/
class Token {
public:

	/*! \enum TokenType
	\brief a public enum defining the possible token types.
	*/
	enum TokenType {
		OPEN,  //< open tag, aka '('
		CLOSE, //< close tag, aka ')'
		SYM, //< symbol tag
		STR, //< string tag, aka '\"'
	};

	/// construct a token of type t (if string default to empty value)
	Token(TokenType t);

	/// contruct a token of type Symbol with value
	Token(const std::string & sym);

	/// construct a token of type String with value
	Token(const std::string & str, bool thing);

	/// return the type of the token
	TokenType type() const;

	/// return the token rendered as a string
	std::string asString() const;
private:
	TokenType m_type;
	std::string value;
	bool string = false;
};

/*! \typedef TokenSequenceType
Define the token sequence using a std container. Any supporting
sequential access should do.
*/
typedef std::deque<Token> TokenSequenceType;

/*! \fn TokenSequenceType tokenize(std::istream & seq)
\brief Split a stream into a sequnce of tokens

\param seq the input character stream
\return The sequence of tokens

Split a stream into a sequnce of tokens where a token is one of
OPEN or CLOSE or any space-delimited string

Ignores any whitespace and comments (from any ";" to end-of-line).
*/
TokenSequenceType tokenize(std::istream & seq);

#endif
