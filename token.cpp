#include "token.hpp"

// system includes
#include <cctype>
#include <iostream>

// define constants for special characters
const char OPENCHAR = '(';
const char CLOSECHAR = ')';
const char COMMENTCHAR = ';';
const char STRCHAR = '\"';

Token::Token(TokenType t) : m_type(t) {}

Token::Token(const std::string & sym) : m_type(SYM), value(sym) {}

Token::Token(const std::string & str, bool thing) : m_type(STR), value(str), string(thing) {};

Token::TokenType Token::type() const {
	return m_type;
}

std::string Token::asString() const {
	switch (m_type) {
	case OPEN:
		return "(";
	case CLOSE:
		return ")";
	case SYM:
		return value;
	case STR:
		return "\"";
	}
	return "";
}

// add token to sequence unless it is empty, clears token
void store_ifnot_empty(std::string & token, TokenSequenceType & seq) {
	if (!token.empty()) {
		seq.emplace_back(token);
		token.clear();
	}
}

TokenSequenceType tokenize(std::istream & seq) {
	TokenSequenceType tokens;
	std::string token;
	int is_inside_quote = 0;
	while (true) {
		char c = seq.get();
		if (seq.eof()) break;

		if (c == COMMENTCHAR) {
			// chomp until the end of the line
			while ((!seq.eof()) && (c != '\n')) {
				c = seq.get();
			}
			if (seq.eof()) break;
		}
		else if (c == OPENCHAR) {
			store_ifnot_empty(token, tokens);
			tokens.push_back(Token::TokenType::OPEN);
		}
		else if (c == CLOSECHAR) {
			store_ifnot_empty(token, tokens);
			tokens.push_back(Token::TokenType::CLOSE);
		}
		else if (c == STRCHAR) {
			store_ifnot_empty(token, tokens);
			tokens.push_back(Token::TokenType::STR);
			if (is_inside_quote == 0) {
				is_inside_quote = 1;
			}
			else {
				is_inside_quote = 0;
			}
		}
		else if (isspace(c) && is_inside_quote != 1) {
			store_ifnot_empty(token, tokens);
		}
		else {
			token.push_back(c);
		}
	}
	store_ifnot_empty(token, tokens);

	return tokens;
}
