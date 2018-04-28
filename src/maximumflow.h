#ifndef MAXIMUMFLOW_H
#define MAXIMUMFLOW_H

#include "calc.h"
#include "graphdata.hpp"

class QComboBox;

class MaximumFlow : public Calc
{
  Q_OBJECT

public:
  MaximumFlow(const GraphData &graph, QWidget *parent = 0);

protected slots:
  void enableOkButton();

protected:
  void createWidgets();
  void createLayouts();
  void createConnects();

  void resultDialogShow(double result);

  QComboBox *startID;
  QComboBox *endID;

private:
  QLabel *startIDLabel;
  QLabel *endIDLabel;
  QDialogButtonBox *buttonBox;

  QDialog *resultDialog;
};

#endif
