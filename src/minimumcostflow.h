#ifndef MINIMUMCOSTFLOW_H
#define MINIMUMCOSTFLOW_H

#include "calc.h"
#include "graphdata.hpp"

class QLabel;
class QLineEdit;
class QComboBox;
class QDialogButtonBox;

class MinimumCostFlow : public Calc
{
  Q_OBJECT

public:
  MinimumCostFlow(const GraphData &graph, QWidget *parent = 0);

private slots:
  void calc();
  void enableOkButton();

private:
  void createWidgets();
  void createLayouts();
  void createConnects();

  void resultDialogShow(double result);

  QLabel *startIDLabel;
  QLabel *endIDLabel;
  QLabel *flowLabel;

  QComboBox *startID;
  QComboBox *endID;
  QLineEdit *flowLineEdit;
  QDialogButtonBox *buttonBox;

  QDialog *resultDialog;
};

#endif
