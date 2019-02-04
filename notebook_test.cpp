#include <QTest>
#include "notebook_app.hpp"
class NotebookTest : public QObject {
  Q_OBJECT

private slots:

  void initTestCase();
  void testFindByName();
  void testMakePoint();
  void testMakeLine();
  void testMakeText();
  void testExpression();
  void testDiscretePlotLayout();
  void testSimpleDiscretePlot();
  void randomContinuous();

  // TODO: implement additional tests here
private:

  NotebookApp *widget;
  InputWidget *inputWidget;
  OutputWidget *outputWidget;
};

void NotebookTest::initTestCase(){
	widget = new NotebookApp;
	inputWidget = widget->findChild<InputWidget *>(QString("input"));
	outputWidget = widget->findChild<OutputWidget *>(QString("output"));
	widget->show();
}

void NotebookTest::testFindByName() {
	auto op = widget->findChild<InputWidget *>(QString("input"));
	QVERIFY2(op, "could not find input widget");
	auto op1 = widget->findChild<OutputWidget *>(QString("output"));
	QVERIFY2(op1, "could not find output widget");
}

void NotebookTest::testMakePoint() {
	auto op = widget->findChild<InputWidget *>(QString("input"));
	auto op1 = widget->findChild<OutputWidget *>(QString("output"));
	QVERIFY2(op1, "no output");

	QTest::keyClicks(op, "(make-point 0 0)");
	QTest::keyPress(op, Qt::Key_Shift);
	QTest::keyPress(op, Qt::Key_Return);
	QTest::keyRelease(op, Qt::Key_Return);
	QTest::keyRelease(op, Qt::Key_Shift);

	auto op2 = op1->get_scene();
	QVERIFY2(op2, "no");

	QVERIFY2(op2->itemAt(QPointF(0, 0), QTransform()), "no item");
}

void NotebookTest::testMakeLine() {
	auto op = widget->findChild<InputWidget *>(QString("input"));
	auto op1 = widget->findChild<OutputWidget *>(QString("output"));

	QTest::keyClicks(op, "(make-line (make-point 0 0) (make-point 0 20))");
	QTest::keyPress(op, Qt::Key_Shift);
	QTest::keyPress(op, Qt::Key_Return);
	QTest::keyRelease(op, Qt::Key_Return);
	QTest::keyRelease(op, Qt::Key_Shift);

	auto op2 = op1->get_scene();
	QVERIFY2(op2->itemAt(QPointF(0, 0), QTransform()), "no endpoint 1");
	QVERIFY2(op2->itemAt(QPointF(0, 20), QTransform()), "no endpoint 2");
}

void NotebookTest::testMakeText() {
	auto op = widget->findChild<InputWidget *>(QString("input"));
	auto op1 = widget->findChild<OutputWidget *>(QString("output"));

	QTest::keyClicks(op, "(make-text \"foo\")");
	QTest::keyPress(op, Qt::Key_Shift);
	QTest::keyPress(op, Qt::Key_Return);
	QTest::keyRelease(op, Qt::Key_Return);
	QTest::keyRelease(op, Qt::Key_Shift);

	auto op2 = op1->get_scene();
	QVERIFY2(op2->itemAt(QPointF(0, 0), QTransform()), "no text");
}

void NotebookTest::testExpression() {
	auto op = widget->findChild<InputWidget *>(QString("input"));
	auto op1 = widget->findChild<OutputWidget *>(QString("output"));

	QTest::keyClicks(op, "(+ 1 2)");
	QTest::keyPress(op, Qt::Key_Shift);
	QTest::keyPress(op, Qt::Key_Return);
	QTest::keyRelease(op, Qt::Key_Return);
	QTest::keyRelease(op, Qt::Key_Shift);

	auto op2 = op1->get_scene();
	QVERIFY2(op2->itemAt(QPointF(0, 0), QTransform()), "no expression");
}

/*
findLines - find lines in a scene contained within a bounding box
with a small margin
*/
int findLines(QGraphicsScene * scene, QRectF bbox, qreal margin) {

	QPainterPath selectPath;

	QMarginsF margins(margin, margin, margin, margin);
	selectPath.addRect(bbox.marginsAdded(margins));
	scene->setSelectionArea(selectPath, Qt::ContainsItemShape);

	int numlines(0);
	for (auto item: scene->selectedItems()) {
		if (item->type() == QGraphicsLineItem::Type) {
			numlines += 1;
		}
	}

	return numlines;
}

/*
findPoints - find points in a scene contained within a specified rectangle
*/
int findPoints(QGraphicsScene * scene, QPointF center, qreal radius) {

	QPainterPath selectPath;
	selectPath.addRect(QRectF(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius));
	scene->setSelectionArea(selectPath, Qt::ContainsItemShape);

	int numpoints(0);
	for (auto item: scene->selectedItems()) {
		if (item->type() == QGraphicsEllipseItem::Type) {
			numpoints += 1;
		}
	}

	return numpoints;
}

/*
findText - find text in a scene centered at a specified point with a given
rotation and string contents
*/
int findText(QGraphicsScene * scene, QPointF center, qreal rotation, QString contents) {

	int numtext(0);
	for (auto item: scene->items(center)) {
		if (item->type() == QGraphicsTextItem::Type) {
			QGraphicsTextItem * text = static_cast<QGraphicsTextItem *>(item);
			if ((text->toPlainText() == contents) &&
				(text->rotation() == rotation) &&
				(text->pos() + text->boundingRect().center() == center)) {
				numtext += 1;
			}
		}
	}

	return numtext;
}

/*
intersectsLine - find lines in a scene that intersect a specified rectangle
*/
int intersectsLine(QGraphicsScene * scene, QPointF center, qreal radius) {

	QPainterPath selectPath;
	selectPath.addRect(QRectF(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius));
	scene->setSelectionArea(selectPath, Qt::IntersectsItemShape);

	int numlines(0);
	for (auto item: scene->selectedItems()) {
		if (item->type() == QGraphicsLineItem::Type) {
			numlines += 1;
		}
	}

	return numlines;
}

void NotebookTest::testDiscretePlotLayout() {
	std::string program = R"( 
(discrete-plot (list (list -1 -1) (list 1 1)) 
    (list (list "title" "The Title") 
          (list "abscissa-label" "X Label") 
          (list "ordinate-label" "Y Label") ))
)";

	inputWidget->setPlainText(QString::fromStdString(program));
	QTest::keyClick(inputWidget, Qt::Key_Return, Qt::ShiftModifier);

	auto view = outputWidget->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	QCOMPARE(items.size(), 17);
	
	// make them all selectable
	for (auto item: items) {
		item->setFlag(QGraphicsItem::ItemIsSelectable);
	}

	double scalex = 20.0 / 2.0;
	double scaley = 20.0 / 2.0;

	double xmin = scalex * -1; // -10
	double xmax = scalex * 1; // 10
	double ymin = scaley * -1; // -10
	double ymax = scaley * 1; // 10
	double xmiddle = (xmax + xmin) / 2; // 0
	double ymiddle = (ymax + ymin) / 2; // 0

	// check title
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymax + 3)), 0, QString("The Title")), 1);

	// check abscissa label
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymin - 3)), 0, QString("X Label")), 1);

	// check ordinate label
	QCOMPARE(findText(scene, QPointF(xmin - 3, -ymiddle), -90, QString("Y Label")), 1);

	// check abscissa min label
	QCOMPARE(findText(scene, QPointF(xmin, -(ymin - 2)), 0, QString("-1")), 1);

	// check abscissa max label
	QCOMPARE(findText(scene, QPointF(xmax, -(ymin - 2)), 0, QString("1")), 1);

	// check ordinate min label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymin), 0, QString("-1")), 1);

	// check ordinate max label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymax), 0, QString("1")), 1);

	// check the bounding box bottom
	QCOMPARE(findLines(scene, QRectF(xmin, -ymin, 20, 0), 0.1), 1);

	// check the bounding box top
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 20, 0), 0.1), 1);

	// check the bounding box left and (-1, -1) stem
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 0, 20), 0.1), 2);

	// check the bounding box right and (1, 1) stem
	QCOMPARE(findLines(scene, QRectF(xmax, -ymax, 0, 20), 0.1), 2);

	// check the abscissa axis
	QCOMPARE(findLines(scene, QRectF(xmin, 0, 20, 0), 0.1), 1);

	// check the ordinate axis 
	QCOMPARE(findLines(scene, QRectF(0, -ymax, 0, 20), 0.1), 1);

	// check the point at (-1,-1)
	QCOMPARE(findPoints(scene, QPointF(-10, 10), 0.6), 1);

	// check the point at (1,1)
	QCOMPARE(findPoints(scene, QPointF(10, -10), 0.6), 1);
}

void NotebookTest::testSimpleDiscretePlot() {
	std::string program = "(begin (define f (lambda (x) (list x (+ (* 2 x) 1)))) (discrete-plot (map f (range -2 2 0.5)) (list (list \"title\" \"The Data\") (list \"abscissa-label\" \"X Label\") (list \"ordinate-label\" \"Y Label\") (list \"text-scale\" 1))))";

	inputWidget->setPlainText(QString::fromStdString(program));
	QTest::keyClick(inputWidget, Qt::Key_Return, Qt::ShiftModifier);

	auto view = outputWidget->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");
	view->show();
	widget->show();
	auto scene = view->scene();

	auto items = scene->items();
	int text_count = 0, line_count = 0, point_count = 0;
	for (auto item : items) {
		if (item->type() == QGraphicsTextItem::Type) {
			QGraphicsTextItem * text = static_cast<QGraphicsTextItem *>(item);
			qDebug() << text->toPlainText();
			text_count++;
		}
		else if (item->type() == QGraphicsLineItem::Type) {
			line_count++;
		}
		else if (item->type() == QGraphicsEllipseItem::Type) {
			point_count++;
			QGraphicsEllipseItem *ellipse = static_cast<QGraphicsEllipseItem *>(item);
			qDebug() << ellipse->rect();
		}
	}
	qDebug() << "text " << text_count << "lines " << line_count << "points " << point_count;
	QCOMPARE(items.size(), 31);
}

void NotebookTest::randomContinuous() {
	std::string program = "(begin (define f (lambda (x) (+ (* 2 x) 1))) (continuous-plot f (list -2 2) (list (list \"title\" \"A continuous linear function\") (list \"abscissa - label\" \"x\") (list \"ordinate - label\" \"y\"))))";

	inputWidget->setPlainText(QString::fromStdString(program));
	QTest::keyClick(inputWidget, Qt::Key_Return, Qt::ShiftModifier);

	auto view = outputWidget->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	auto items = scene->items();
	for (auto item : items) {
		if (item->type() == QGraphicsEllipseItem::Type) {
			QGraphicsEllipseItem *ellipse = static_cast<QGraphicsEllipseItem *>(item);
			qDebug() << ellipse->rect();
		}
	}
}
QTEST_MAIN(NotebookTest)
#include "notebook_test.moc"
