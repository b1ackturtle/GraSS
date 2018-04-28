#ifndef EULERPATH_H
#define EULERPATH_H

#include "calc.h"
#include "graphdata.hpp"

class EulerPath : public Calc
{
Q_OBJECT

public:
  EulerPath(const GraphData &graph, QWidget *parent = 0);

private slots:
  void calc();

private:
  void createWidgets();
  void createLayouts();
  void createConnects();

  void resultDialogShow(const bool isEuler);
  void resultDialogShow(const bool isEuler,
			const long long int start, const long long int end);

  QLabel *cycleCheckLabel;
  QCheckBox *cycleCheckBox;
  QDialogButtonBox *buttonBox;

  QDialog *resultDialog;
};

#endif
