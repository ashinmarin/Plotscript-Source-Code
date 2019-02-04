#include "parse.hpp"
#include "parse.hpp"

#include <stack>

bool setHead(Expression &exp, const Token &token) {

	Atom a(token, false);

	exp.head() = a;

	return !a.isNone();
}

bool setStringHead(Expression &exp, const Token &token) {
	Atom a(token, true);
	exp.head() = a;
	return !a.isNone();
}

bool stringAppend(Expression *exp, const Token &token) {
	Atom a(token, true);
	exp->append(a);
	return !a.isNone();
}

bool append(Expression *exp, const Token &token) {

	Atom a(token, false);

	exp->append(a);

	return !a.isNone();
}

Expression parse(const TokenSequenceType &tokens) noexcept {

	Expression ast;

	// cannot parse empty
	if (tokens.empty())
		return Expression();

	bool athead = false;
	bool atstringhead = false;
	bool stringhead = false;
	bool inquote = false;
	// stack tracks the last node created
	std::stack<Expression *> stack;

	std::size_t num_tokens_seen = 0;

	for (auto &t : tokens) {

		if (t.type() == Token::OPEN) {
			athead = true;
		}
		else if (t.type() == Token::CLOSE) {
			if (stack.empty()) {
				return Expression();
			}
			stack.pop();

			if (stack.empty()) {
				num_tokens_seen += 1;
				break;
			}
		}
		else if (t.type() == Token::STR) {
			if (!inquote) {
				inquote = true;
				atstringhead = true;
			}
			else {
				if (stack.empty()) {
					return Expression();
				}
				if (!stringhead) {
					stack.pop();
				}
				if (stack.empty()) {
					num_tokens_seen += 1;
				}
				inquote = false;
			}
		}
		else {
			if (athead && !atstringhead) {
				if (stack.empty()) {
					if (!setHead(ast, t)) {
						return Expression();
					}
					stack.push(&ast);
				}
				else {
					if (stack.empty()) {
						return Expression();
					}

					if (!append(stack.top(), t)) {
						return Expression();
					}
					stack.push(stack.top()->tail());
				}
				athead = false;
			}
			else if (atstringhead && !athead) {
				if (stack.empty()) {
					stringhead = true;
					if (!setStringHead(ast, t)) {
						return Expression();
					}
					stack.push(&ast);
				}
				else {
					if (stack.empty()) {
						return Expression();
					}

					if (!stringAppend(stack.top(), t)) {
						return Expression();
					}
					stack.push(stack.top()->tail());
				}
				atstringhead = false;
			}
			else if (athead && atstringhead) {
				stringhead = true;
				if (stack.empty()) {
					if (!setStringHead(ast, t)) {
						return Expression();
					}
					stack.push(&ast);
				}
				else {
					if (stack.empty()) {
						return Expression();
					}

					if (!stringAppend(stack.top(), t)) {
						return Expression();
					}
					stack.push(stack.top()->tail());
				}
				athead = false;
				atstringhead = false;
			}
			else {
				if (stack.empty()) {
					return Expression();
				}

				if (!append(stack.top(), t)) {
					return Expression();
				}
			}
		}
		num_tokens_seen += 1;
	}

	if (stack.empty() && (num_tokens_seen == tokens.size())) {
		return ast;
	}

	return Expression();
};
