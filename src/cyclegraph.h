#ifndef CYCLEGRAPH_H
#define CYCLEGRAPH_H

#include <QDialog>
#include "insertgraph.h"

class GraphicsView;
class QLabel;
class QLineEdit;
class QDialogButtonBox;

class CycleGraph : public InsertGraph
{
public:
  CycleGraph(GraphicsView *view, QWidget *parent = 0);

  void run(long long int N, long long int R);
};

class CycleGraphDialog : public QDialog
{
  Q_OBJECT

public:
  CycleGraphDialog(QWidget *parent = 0);
  QLineEdit *nodeCountLineEdit;
  QLineEdit *radiusLineEdit;

private slots:
  void enableOkButton();

private:
  void createWidgets();
  void createLayouts();
  void createConnects();

  QLabel *nodeCountLabel;
  QLabel *radiusLabel;
  QDialogButtonBox *buttonBox;
};

#endif
