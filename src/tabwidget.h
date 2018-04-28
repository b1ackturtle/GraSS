#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>
#include <QObject>

#include "graphdata.hpp"

class TabBar;
class QAction;
class QFile;
class QGraphicsItem;
class GraphicsView;
class QTextStream;

class TabWidget : public QTabWidget
{
  Q_OBJECT

public:
  TabWidget(QWidget *parent);

  QAction *newTabAction() const;
  QAction *closeTabAction() const;
  QAction *nextTabAction() const;
  QAction *previousTabAction() const;

  bool isModified(int index);

signals:
  void lastTabClosed();

public slots:
  void newTab();
  void whiteNewTab();
  void nextTab();
  void previousTab();
  void closeTab(int index = -1);
  void closeOtherTabs(int index);
  void cloneTab(int index);
  void renameTab(int index);
  void modifiedTab(bool flag);

  void changeView(int index);
  
  void fileOpen();
  bool fileSave();
  bool fileSaveAs();
  void fileImport();
  void fileExport();
  void loadFile();

private:
  bool okToClearData(int index);
  bool openGraSSFile(QFile *file, QDataStream &in);
  bool openImportFile(QFile *file, long long int &N, long long int &E,
		      std::vector< Edge > &edges);
  void readItems(QDataStream &in);
  void writeItems(QDataStream &out, const QList<QGraphicsItem*> &items);
  void exportImage(const QString &filename);
  void exportSvg(const QString &filename);
  void paintScene(QPainter *painter);
  QString filenameFilter(const QString &name, const QList<QByteArray> formats);

  void retranslate();

  QAction *m_newTabAction;
  QAction *m_closeTabAction;
  QAction *m_nextTabAction;
  QAction *m_previousTabAction;

  TabBar *m_tabBar;
  GraphicsView *view;

  QWidget *m_parent;
};

#endif
