#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP
// system includes
#include <istream>
#include <string>
#include <stdexcept>
// module includes
#include "environment.hpp"
#include "expression.hpp"
#include "tsQueue.hpp"
#include "token.hpp"
#include "parse.hpp"
#include "semantic_error.hpp"
#include <iostream>
typedef std::string MessageType;
/*! \class Interpreter
\brief Class to parse and evaluate an expression (program)

Interpreter has an Environment, which starts at a default.
The parse method builds an internal AST.
The eval method updates Environment and returns last result.
*/

struct QueueItem {
	Expression output;
	bool error;
};

class Interpreter {
public:
	Interpreter(); 

	Interpreter(MessageQueue<MessageType> * ichannel, MessageQueue<Expression> * ochannel, MessageQueue<std::string> * echannel, MessageQueue<MessageType> * kernel_cmd_channel , MessageQueue<bool> * kernel_ichannel );

	bool parseStream(std::istream &expression) noexcept;

	/*!
	Interpreter Kernel Thread Function
	*/
	void operator()();

	/*
	Connect multithreading communication channels
	*/
	void connect_func(MessageQueue<MessageType> * ichannel, MessageQueue<Expression> * ochannel, MessageQueue<std::string> * echannel, MessageQueue<MessageType> * kernel_cmd_channel); 

	/*! Evaluate the Expression by waking up, returning the result.
	  \return the Expression resulting from the evaluation in the current environment
	  \throws SemanticError when a semantic error is encountered
	 */
	Expression evaluate();

	void setGUI(); 

private:

	//Input Message Queue
	MessageQueue<MessageType> * iqueue;

	//output Message Queue
	MessageQueue<Expression> *  oqueue;


	//Exception Message Queue
	MessageQueue<std::string> *  equeue;

	//Kernel Command QUeue
	MessageQueue<MessageType> * kernel_cmd; 

	MessageQueue<bool> * itqueue; 

  // the environment
	Environment env;

	// the AST
	Expression ast;

	bool gui = false; 
};
#endif