#include <QApplication>
#include <QWidget>
#include "notebook_app.hpp"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  NotebookApp widget; //this is the main window in which 2 subwindows appear

  widget.show();
  
  return app.exec();
}

