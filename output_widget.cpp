#include "output_widget.hpp"
#include <QGridLayout>

OutputWidget::OutputWidget() {
	scene = new QGraphicsScene();
	view = new QGraphicsView(scene);
	view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
	auto layout = new QGridLayout();
	layout->addWidget(view);
	setLayout(layout);
}

void OutputWidget::resize_view() {
	view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void OutputWidget::show_scene(QGraphicsTextItem * str, bool clear) {
	if (clear) {
		scene->clear();
	}
	scene->addItem(str);
	resize_view();
	view->update();
}

void OutputWidget::show_scene(QGraphicsEllipseItem *ellip, bool clear) {
	if (clear) {
		scene->clear();
	}
	ellip->setBrush(QBrush(Qt::SolidPattern));
	scene->addItem(ellip);
	resize_view();
	view->update();
}

void OutputWidget::show_scene(QGraphicsLineItem *line, bool clear) {

	if (clear) {
		scene->clear();
	}
	scene->addItem(line);
	resize_view();
	view->update();
}
