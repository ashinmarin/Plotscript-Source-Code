#ifndef NOTEBOOK_APP_HPP
#define NOTEBOOK_APP_HPP

#include <QAction>
#include <QGraphicsTextItem>
#include <QGridLayout>
#include <QPushButton>
#include <QWidget>

#include <fstream>
#include <sstream>

#include "interpreter.hpp"
#include "input_widget.hpp"
#include "output_widget.hpp"
#include "tsQueue.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"

#include <thread>
class NotebookApp : public QWidget {
Q_OBJECT
public:
	NotebookApp();
	~NotebookApp();

	public slots:
	void eval_plotscript();
	//Expression eval_from_stream(std::istream & stream);
	void start_kernel();
	void stop_kernel();
	void reset_kernel();

private:
	InputWidget *input;
	OutputWidget *output;

	QShortcut *eval_shortcut;
	Interpreter interp1;
	
	MessageQueue<std::string> input_q;
	MessageQueue<Expression> output_q;
	//Message Queue to pass excepetions to TUI
	MessageQueue<std::string> echannel;
//	MessageQueue<Expression> eval_q;

	//message queue to pass kernel command from TUI
MessageQueue<MessageType> kernel_cmd_channel;

//message channel for interrupts
MessageQueue<bool> kernel_ichannel;
	std::thread producing;
	bool interp_running;
	
	QPushButton *start_button;
	QPushButton *stop_button;
	QPushButton *reset_button;
	QPushButton *interrupt_button;

	void evaluate_it(Expression exp, bool first);
	void output_command(std::string str, bool clear);
	void text_output(std::string str, bool clear);
	void point_create(Expression exp, bool clear);
	void line_create(Expression exp, bool clear);
	void text_add(Expression exp, bool clear);
};
#endif