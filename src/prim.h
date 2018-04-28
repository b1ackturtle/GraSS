#ifndef PRIM_H
#define PRIM_H

#include "calc.h"

class GraphData;

class Prim : public Calc
{
  Q_OBJECT

public:
  Prim(const GraphData &graph, QWidget *parent = 0);

  void calc();

private:
  void resultDialogShow();

  std::vector< std::vector< double > > graph;

  double result;

  QDialog *resultDialog;
  QDialogButtonBox *buttonBox;
};

#endif
