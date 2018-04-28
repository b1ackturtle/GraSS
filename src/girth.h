#ifndef GIRTH_H
#define GIRTH_H

#include "calc.h"
#include "graphdata.hpp"

class Girth : public Calc
{
public:
  Girth(const GraphData &graph, QWidget *parent = 0);

  void calc();

private:
  void resultDialogShow(const double result, const std::vector< long long int > &path);

  QDialog *resultDialog;
};

#endif
