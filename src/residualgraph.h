#ifndef RESIDUALGRAPH_H
#define RESIDUALGRAPH_H

#include "calc.h"
#include "graphdata.hpp"

class ResidualGraph : public Calc
{
public:
  ResidualGraph(const GraphData &graph, QWidget *parent = 0);

  void calc();
};

#endif
