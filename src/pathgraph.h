#ifndef PATHGRAPH_H
#define PATHGRAPH_H

#include <QDialog>
#include "insertgraph.h"

class GraphicsView;
class QLabel;
class QLineEdit;
class QComboBox;
class QDialogButtonBox;

class PathGraph : public InsertGraph
{
public:
  PathGraph(GraphicsView *view, QWidget *parent = 0);

  void run(long long int N, long long int interval, bool axis);
};

class PathGraphDialog : public QDialog
{
  Q_OBJECT

public:
  PathGraphDialog(QWidget *parent = 0);

  QLineEdit *nodeCountLineEdit;
  QLineEdit *intervalLineEdit;
  QComboBox *axisComboBox;

private slots:
  void enableOkButton();

private:
  void createWidgets();
  void createLayouts();
  void createConnects();

  QLabel *nodeCountLabel;
  QLabel *intervalLabel;
  QLabel *axisLabel;
  QDialogButtonBox *buttonBox;
};

#endif
