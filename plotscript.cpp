// This is an example of how to to trap Cntrl-C in a cross-platform manner

//by creating a simple REPL event loop and shows how to interrupt it.
#include <csignal>

#include <cstdlib>
#include <atomic>
// This global is needed for communication between the signal handler and the rest of the code. This atomic integer counts the number of times

volatile sig_atomic_t global_status_flag = 0;
extern std::atomic<bool> interrupt;

// *****************************************************************************

// install a signal handler for Cntl-C on Windows

// *****************************************************************************

#if defined(_WIN64) || defined(_WIN32)

#include <windows.h>

// this function is called when a signal is sent to the process

BOOL WINAPI interrupt_handler(DWORD fdwCtrlType) {
	switch (fdwCtrlType) {

	case CTRL_C_EVENT: // handle Cnrtl-C

	  // if not reset since last call, exit

		if (global_status_flag > 0) {

			exit(EXIT_FAILURE);

		}
		++global_status_flag;

		return TRUE;
	default:

		return FALSE;
	}
}



// install the signal handler

inline void install_handler() { SetConsoleCtrlHandler(interrupt_handler, TRUE); }

// *****************************************************************************

// *****************************************************************************

// install a signal handler for Cntl-C on Unix/Posix

// *****************************************************************************

#elif defined(__APPLE__) || defined(__linux) || defined(__unix) ||  defined(__posix)

#include <unistd.h>

// this function is called when a signal is sent to the process

void interrupt_handler(int signal_num) {

	if (signal_num == SIGINT) { // handle Cnrtl-C

	  // if not reset since last call, exit

		if (global_status_flag > 0) {

			exit(EXIT_FAILURE);
		}
		++global_status_flag;
	}
}

// install the signal handler

inline void install_handler() {
	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = interrupt_handler;

	sigemptyset(&sigIntHandler.sa_mask);

	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);

}

#endif

// *****************************************************************************
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"
#include "tsQueue.hpp"
#include <thread>


#define START "%start"
#define STOP "%stop"
#define RESET "%reset" 
#define EXIT "%exit"


//Message Queue to pass input to interpreter kernel
MessageQueue<MessageType> ichannel;
//Message Queue to pass output expression to TUI
MessageQueue<Expression> ochannel;
//Message Queue to pass excepetions to TUI
MessageQueue<std::string> echannel;

//message queue to pass kernel command from TUI
MessageQueue<MessageType> kernel_cmd_channel;

//message channel for interrupts
MessageQueue<bool> kernel_ichannel;


void prompt() {
	std::cout << "\nplotscript> ";
}

std::string readline() {
	std::string line;
	std::getline(std::cin, line);

	return line;
}

void error(const std::string & err_str) {
	std::cerr << "Error: " << err_str << std::endl;
}

void info(const std::string & err_str) {
	std::cout << "Info: " << err_str << std::endl;
}

int eval_startup(Interpreter & interp)
{
	std::ifstream ifs(STARTUP_FILE);

	if (!ifs) {
		error("Could not open file for reading.");
		return EXIT_FAILURE;
	}

	if (!interp.parseStream(ifs)) {
		error("Invalid Program. Could not parse.");
		return EXIT_FAILURE;
	}
	else {
		try {
			Expression exp = interp.evaluate();
		}
		catch (const SemanticError & ex) {
			std::cerr << ex.what() << std::endl;
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

int eval_from_stream(std::istream & stream) {

	Interpreter interp;

	eval_startup(interp);

	if (!interp.parseStream(stream)) {
		error("Invalid Program. Could not parse.");
		return EXIT_FAILURE;
	}
	else {
		try {
			Expression exp = interp.evaluate();
			std::cout << exp << std::endl;
		}
		catch (const SemanticError & ex) {
			std::cerr << ex.what() << std::endl;
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

int eval_from_file(std::string filename) {

	std::ifstream ifs(filename);

	if (!ifs) {
		error("Could not open file for reading.");
		return EXIT_FAILURE;
	}

	return eval_from_stream(ifs);
}

int eval_from_command(std::string argexp) {

	std::istringstream expression(argexp);

	return eval_from_stream(expression);
}


bool runKernel()
{
	std::string exception;

	while (true) {


		global_status_flag = 0; 
		prompt();
		std::string line = readline();


		if (std::cin.fail() || std::cin.eof()) {

			std::cin.clear(); // reset cin state

			line.clear(); //clear input string

			std::cerr << "Error: interpreter kernel interrupted. \n";
		
		}

		else {
			if (line == STOP) {

			kernel_cmd_channel.push(STOP);
			return false; 
		}
		else if (line == RESET)
		{
			kernel_cmd_channel.push(RESET);
			return true; 

		}
		else if (line == EXIT)
		{
			kernel_cmd_channel.push(EXIT);
			kernel_cmd_channel.push(EXIT);
			return false; 
		}
		//evaluate plotscript code
		else {

			ichannel.push(line);
			Expression exp;
			while (true)
			{
				if (ochannel.try_pop(exp))
				{
					std::cout << exp;
					break;
				}
				else if (echannel.try_pop(exception))
				{
					break;
				}
				else if (global_status_flag > 0) {
					interrupt = true; 
				}
				}

			}
		}
	}

	return false; 
}

// A REPL is a repeated read-eval-print loop
void repl() {

	bool reset = false; 
	bool start = true; 
	
	install_handler();

	while (true) {

		global_status_flag = 0;

		std::string line; 
		if (!reset && !start) 
		{
			prompt();
			line = readline();
			if (std::cin.fail() || std::cin.eof()) {

				std::cin.clear(); // reset cin state

					line.clear(); //clear input string

					continue; 
			}
		}

		if (line == STOP || line == RESET)
			continue;
		if (line == START || reset || start)
		{
			start = false; 
			reset = false; 
			Interpreter interp(&ichannel, &ochannel, &echannel, &kernel_cmd_channel, &kernel_ichannel);
			eval_startup(interp);

			std::thread kernel_thread(interp);

			//Call 
			reset = runKernel(); 
			kernel_thread.join();
			if (!kernel_cmd_channel.empty())
				break; 

		}
		else
		{
			error("interpreter kernel not running");
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc == 2) {
		return eval_from_file(argv[1]);
	}
	else if (argc == 3) {
		if (std::string(argv[1]) == "-e") {
			return eval_from_command(argv[2]);
		}
		else {
			error("Incorrect number of command line arguments.");
		}
	}
	else 
	{	
		repl();
	}

	return EXIT_SUCCESS;
}
