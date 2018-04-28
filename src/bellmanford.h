#ifndef BELLMANFORD_H
#define BELLMANFORD_H

#include "calc.h"
#include "graphdata.hpp"

class QLabel;
class QLineEdit;
class QComboBox;
class QDialogButtonBox;
class QVBoxLayout;
class GraphicsView;
class MainWindow;
class QGraphicsItem;
class QGraphicsScene;

class BellmanFord : public Calc
{
  Q_OBJECT

public:
  BellmanFord(const GraphData &graph, QWidget *parent = 0);

private slots:
  void calc();
  void enableOkButton();

private:
  void createWidgets();
  void createLayouts();
  void createConnects();

  void resultDialogShow();

  std::vector< std::vector<Edge> > distList;
  std::vector< long long int > prev; //for saving a path
  std::vector<long long int> path;

  QLabel *startIDLabel;
  QLabel *endIDLabel;
  QComboBox *startID;
  QComboBox *endID;
  QDialogButtonBox *buttonBox;

  bool negativeCycle;
  double result;

  QDialog *resultDialog;
};

#endif
