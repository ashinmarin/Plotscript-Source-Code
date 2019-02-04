#ifndef INPUT_WIDGET_HPP
#define INPUT_WIDGET_HPP

#include <QDebug>
#include <QLabel>
#include <QObject>
#include <QPlainTextEdit>
#include <QShortcut>
#include <QString>

class InputWidget : public QPlainTextEdit {
  Q_OBJECT

public:
	InputWidget() {};

protected:
	void keyPressEvent(QKeyEvent *e);

private:
	bool shift = false;

signals:
	void evaluate();
};
#endif
