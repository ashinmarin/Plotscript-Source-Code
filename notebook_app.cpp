#include "notebook_app.hpp"

#include <cmath>
#include <iostream>
#include <math.h>

#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QPointF>
#include <QRectF>
#include <QSize>
#include <QSizeF>

#define START "%start"
#define STOP "%stop"
#define RESET "%reset" 
#define EXIT "%exit"

NotebookApp::NotebookApp() {
	input = new InputWidget();
	input->setObjectName("input");

	output = new OutputWidget();
	output->setObjectName("output");

	start_button = new QPushButton("Start Kernel");
	start_button->setObjectName("start");

	stop_button = new QPushButton("Stop Kernel");
	stop_button->setObjectName("stop");

	reset_button = new QPushButton("Reset Kernel");
	reset_button->setObjectName("reset");

	interrupt_button = new QPushButton("Interrupt");

	Interpreter interp(&input_q, &output_q, &echannel, &kernel_cmd_channel, &kernel_ichannel);

	producing = std::thread(interp);
	interp_running = true;
	
	QObject::connect(input, SIGNAL(evaluate()), this, SLOT(eval_plotscript()));
	QObject::connect(start_button, SIGNAL(clicked()), this, SLOT(start_kernel()));
	QObject::connect(stop_button, SIGNAL(clicked()), this, SLOT(stop_kernel()));
	QObject::connect(reset_button, SIGNAL(clicked()), this, SLOT(reset_kernel()));

	auto h_layout = new QHBoxLayout();
	h_layout->addWidget(start_button);
	h_layout->addWidget(stop_button);
	h_layout->addWidget(reset_button);
	h_layout->addWidget(interrupt_button);

	auto layout = new QGridLayout();
	layout->addLayout(h_layout, 0, 0);
	layout->addWidget(input, 1, 0);
	layout->addWidget(output, 2, 0);
	
	setLayout(layout);
}

NotebookApp::~NotebookApp() {
	if (interp_running) {
		kernel_cmd_channel.push(STOP);
		producing.join();
	}
}

void NotebookApp::start_kernel() {
	if (!interp_running) {
		Interpreter interp(&input_q, &output_q, &echannel, &kernel_cmd_channel, &kernel_ichannel);
		producing = std::thread(interp);
		interp_running = true;
	}
}

void NotebookApp::stop_kernel() {
	if (interp_running) {
		kernel_cmd_channel.push(STOP);
		interp_running = false;
		producing.join();
	}
}

void NotebookApp::reset_kernel() {
	interp1 = Interpreter();
	if (interp_running) {
		kernel_cmd_channel.push(STOP);
		producing.join();
	}
	Interpreter interp(&input_q, &output_q, &echannel, &kernel_cmd_channel, &kernel_ichannel);
	producing = std::thread(interp);
	interp_running = true;
}

void NotebookApp::evaluate_it(Expression exp, bool first) {
	Expression obj(Atom("object-name", true));
	Expression point(Atom("point", true));
	Expression line(Atom("line", true));
	Expression text(Atom("text", true));

	if (exp.isTailEmpty() && !exp.isList() && exp.get_value(obj) != Expression(text)) {
		std::ostringstream oss;
		oss << exp;
		output_command(oss.str(), first);
	}
	else if (exp.get_value(obj) == Expression(point)) {
		point_create(exp, first);
	}
	else if (exp.get_value(obj) == Expression(line)) {
		line_create(exp, first);
	}
	else if (exp.get_value(obj) == Expression(text)) {
		text_add(exp, first);
	}
	else if (exp.isList()) {
		for (unsigned int i = 0; i < exp.getTail().size(); ++i) {
			if (i == 0) {
				evaluate_it(exp.getTail()[i], true);
			}
			else {
				evaluate_it(exp.getTail()[i], false);
			}
		}
	}
}

void NotebookApp::eval_plotscript() {
	std::string command = input->toPlainText().toLocal8Bit().constData();
	std::string exception;
	input_q.push(command);
	Expression qi;
	while (interp_running) {
		if (echannel.try_pop(exception))
		{
			output_command("Error: Invalid command", true);
			break;
		}
		if (output_q.try_pop(qi)) {

			evaluate_it(qi, true);
			break;
		}
	}
	if (!interp_running && command != "") {
		output_command("Error: interpreter kernel not running", true);
	}
}

void NotebookApp::output_command(std::string str, bool clear) {
	QGraphicsTextItem * msg = new QGraphicsTextItem(str.c_str());
	msg->setPos(QPointF(0, 0));
	output->show_scene(msg, clear);
}

void NotebookApp::text_output(std::string str, bool clear) {
	QGraphicsTextItem * msg = new QGraphicsTextItem(str.c_str());
	QRectF zs = msg->boundingRect();
	msg->setPos(QPointF(0 - zs.width()/2, 0 - zs.height()/2));
	auto font = QFont("Monospace");
	font.setStyleHint(QFont::TypeWriter);
	font.setPointSize(1);
	msg->setFont(font);
	output->show_scene(msg, clear);
}
void NotebookApp::point_create(Expression exp, bool clear) {
	std::vector<Expression> points = exp.getTail();
	if (points.size() != 2) {
		output_command("Error: Wrong number of arguments for point creation",true);
	}
	else if (!points[0].isHeadNumber() || !points[1].isHeadNumber()) {
		output_command("Error: Arguments of point must be a number",true);
	}
	else {
		Expression sze(Atom("size", true));
		double size;
		bool error = false;
		if (exp.is_value(sze)) {
			size = exp.get_value(sze).head().asNumber();
			if (size < 0) {
				output_command("Error: size of point must be positive",true);
				error = true;
			}
		}
		else {
			size = 1;
		}
		if (!error) {
			QPen *pen = new QPen;
			pen->setStyle(Qt::SolidLine);
			pen->setWidthF(0);
			pen->setBrush(Qt::SolidPattern);

			QRectF *pointsze = new QRectF(QPointF(points[0].head().asNumber() - size / 2, points[1].head().asNumber() - size / 2), QSizeF(size, size));
			QGraphicsEllipseItem *to_point = new QGraphicsEllipseItem(*pointsze);
			to_point->setPen(*pen);
			output->show_scene(to_point, clear);
		}
	}
}

void NotebookApp::line_create(Expression exp, bool clear) {
	std::vector<Expression> points = exp.getTail();
	Expression obj(Atom("object-name", true));
	Expression point(Atom("point", true));
	Expression thick(Atom("thickness", true));

	if (points.size() != 2) {
		output_command("Error: Wrong number of arguments for line creation",true);
	}
	else if (points[0].get_value(obj) != Expression(point) || points[1].get_value(obj) != Expression(point)) {
		output_command("Error: Arguments of line must be points",true);
	}
	else {
		std::size_t t;
		bool error = false;
		Expression thick(Atom("thickness", true));
		if (exp.is_value(thick)) {
			Expression temp = exp.get_value(thick);
			t = temp.head().asNumber();
		}
		else {
			t = 0;
		}
		if (!error) {
			QPen *pen = new QPen;
			pen->setStyle(Qt::SolidLine);
			pen->setWidth(t);
			pen->setBrush(Qt::black);
			QGraphicsLineItem *line1 = new QGraphicsLineItem(points[0].getTail()[0].head().asNumber(), points[0].getTail()[1].head().asNumber(), points[1].getTail()[0].head().asNumber(), points[1].getTail()[1].head().asNumber());
			line1->setPen(*pen);
			output->show_scene(line1, clear);
		}
	}
}

void NotebookApp::text_add(Expression exp, bool clear) {
	Expression posit(Atom("position", true));
	Expression obj(Atom("object-name", true));
	Expression pt(Atom("point", true));
	Expression scale(Atom("text-scale", true));
	Expression rotate(Atom("text-rotation", true));

	if (!exp.isTailEmpty()) {
		output_command("Error: in text_add",true);
	}
	else if (!exp.head().isString()) {
		output_command("Error: Input must be a string",true);
	}
	else {
		bool error = false;
		std::vector<Expression> ptvalues;
		double text_scale;
		int text_rotation;
		if (exp.is_value(posit)) {
			Expression point = exp.get_value(posit);
			if (point.get_value(obj) == Expression(pt) && point.getTail().size() == 2 && point.getTail()[0].isHeadNumber() && point.getTail()[1].isHeadNumber()) {
				ptvalues = point.getTail();
			}
			else {
				error = true;
			}
		}
		else {
			ptvalues.emplace_back(0);
			ptvalues.emplace_back(0);
		}

		if (exp.is_value(scale) && exp.get_value(scale).isHeadNumber() && exp.get_value(scale).head().asNumber() > 0) {
			text_scale = exp.get_value(scale).head().asNumber();
		}
		else {
			text_scale = 1;
		}

		double pi = -std::acos(-1);
		// setting text-rotation value
		if (exp.is_value(rotate) && exp.get_value(rotate).isHeadNumber() && exp.get_value(rotate).head().asNumber() >= 0 && exp.get_value(rotate).head().asNumber() <= -2 * pi) {
			text_rotation = (int)(exp.get_value(rotate).head().asNumber() * 180 / pi);
		}
		else {
			text_rotation = 0;
		}


		if (!error) {
			QString *text = new QString(exp.head().asSymbol().c_str());
			QGraphicsTextItem *to_text = new QGraphicsTextItem(*text);
			auto font = QFont("Monospace");
			font.setStyleHint(QFont::TypeWriter);
			font.setPointSize(1);
			to_text->setFont(font);
			// centering

			QRectF t_size = to_text->boundingRect();
			to_text->setTransformOriginPoint(QPointF(t_size.width() / 2, t_size.height() / 2));

			to_text->setRotation(text_rotation);
			to_text->setScale(text_scale);



			to_text->setPos(QPointF(ptvalues[0].head().asNumber() - t_size.width() / 2, ptvalues[1].head().asNumber() - t_size.height() / 2));
			output->show_scene(to_text, clear);
		}
	}
}