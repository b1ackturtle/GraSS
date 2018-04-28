#ifndef WARSHALLFLOYD_H
#define WARSHALLFLOYD_H

#include "calc.h"

class QLabel;
class QLineEdit;
class QComboBox;
class QDialogButtonBox;
class QVBoxLayout;
class GraphicsView;
class MainWindow;
class QGraphicsItem;
class QGraphicsScene;

class WarshallFloyd : public Calc
{
  Q_OBJECT

public:
  WarshallFloyd(const GraphData &graph, QWidget *parent = 0);

private slots:
  void calc();
  void enableOkButton();

private:
  void createWidgets();
  void createLayouts();
  void createConnects();

  void resultDialogShow();

  std::vector< std::vector<double> > distMatrix;
  std::vector< std::vector<long long int> > prev; //for saving a path
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
