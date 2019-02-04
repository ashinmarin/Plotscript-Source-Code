#include "interpreter.hpp"
#define START "%start"
#define STOP "%stop"
#define RESET "%reset" 
#define EXIT "%exit"

Interpreter::Interpreter()
{
	iqueue = nullptr; 
	oqueue = nullptr; 
	equeue = nullptr;
	kernel_cmd = nullptr;
	itqueue = nullptr;
}

Interpreter::Interpreter(MessageQueue<MessageType>* ichannel, MessageQueue<Expression>* ochannel, MessageQueue<std::string> * echannel, MessageQueue<MessageType> * kernel_cmd_channel, MessageQueue<bool> * kernel_ichannel)
{
	iqueue = ichannel;
	oqueue = ochannel; 
	equeue = echannel; 
	kernel_cmd = kernel_cmd_channel; 
	itqueue = kernel_ichannel;
}

bool Interpreter::parseStream(std::istream & expression) noexcept{

  TokenSequenceType tokens = tokenize(expression);

  ast = parse(tokens);

  return (ast != Expression());
}
		
void Interpreter::operator()()
{
	std::string exception = "false"; 
	while (true) 
	{
		std::string input;

		if (kernel_cmd->try_pop(input))
		{
			if (input == STOP || input==RESET|| input==EXIT)
			{
				break; 
			}

		}
		else if (iqueue->try_pop(input))
		{
			std::istringstream expression(input);
			if (!parseStream(expression))
			{	
				if(!gui)
				std::cerr << "Error: " << "Invalid Program. Could not parse." << std::endl;
				exception = "parse"; 
				equeue->push(exception); 
			}
			else
			{
				try {
					Expression exp = evaluate();
					oqueue->push(exp);
				}
				catch (const SemanticError & ex)
				{				
					if(!gui)
					std::cerr << "Error: " << ex.what() << std::endl;
					
					exception = ex.what();
					equeue->push(exception); 
				}
			}
		}
	}
}
void Interpreter::connect_func(MessageQueue<MessageType>* ichannel, MessageQueue<Expression>* ochannel, MessageQueue<std::string>* echannel, MessageQueue<MessageType>* kernel_cmd_channel)
{
	iqueue = ichannel;
	oqueue = ochannel;
	equeue = echannel;
	kernel_cmd = kernel_cmd_channel;
}
;

Expression Interpreter::evaluate(){

  return ast.eval(env);
}

void Interpreter::setGUI()
{

	gui = true; 
}
