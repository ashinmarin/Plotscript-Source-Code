#include"input_widget.hpp"

void InputWidget::keyPressEvent(QKeyEvent *e) {

	if ((e->modifiers() & Qt::ShiftModifier) && ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))){
		emit evaluate();
	}
	QPlainTextEdit::keyPressEvent(e);
}


