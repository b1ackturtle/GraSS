#include <QApplication>

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "mainwindow.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  MainWindow mainWindow;

  #if defined(Q_OS_MAC)
  mainWindow.resize(1285, 810);
  app.setWindowIcon(QIcon("://image/GraSS.icns"));
  mainWindow.setWindowIcon(QIcon("://image/GraSS.icns"));
  #elif defined(Q_OS_WIN)
  mainWindow.resize(1295, 830);
  app.setWindowIcon(QIcon("://image/GraSS.ico"));
  mainWindow.setWindowIcon(QIcon("://image/GraSS.ico"));
  #endif

  mainWindow.show();

  return app.exec();
}
