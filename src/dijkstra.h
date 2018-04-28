#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "calc.h"
#include "graphdata.hpp"

class QLabel;
class QComboBox;
class QDialogButtonBox;
class QVBoxLayout;
class GraphicsView;
class MainWindow;
class QGraphicsItem;
class QGraphicsScene;

class Dijkstra : public Calc
{
  Q_OBJECT

public:
  Dijkstra(const GraphData &graph, QWidget *parent = 0);

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

  double result;

  QDialog *resultDialog;
};

#endif
