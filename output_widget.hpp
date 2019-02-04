#ifndef OUTPUT_WIDGET_HPP
#define OUTPUT_WIDGET_HPP

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QString>
#include <QWidget>

class OutputWidget : public QWidget {
	Q_OBJECT
public:
	OutputWidget();

	void resize_view();
	void show_scene(QGraphicsTextItem * str, bool clear);

	void show_scene(QGraphicsEllipseItem * ell, bool clear);

	void show_scene(QGraphicsLineItem * line, bool clear);

	QGraphicsScene * get_scene() {
		return scene;
	}

	QGraphicsView * getView() {
		return view;
	}

private:
	QGraphicsScene * scene;
	QGraphicsView * view;
};
#endif