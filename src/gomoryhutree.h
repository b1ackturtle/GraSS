#ifndef GOMORYHUTREE_H
#define GOMORYHUTREE_H

#include "calc.h"
#include "graphdata.hpp"

class GomoryHuTree : public Calc
{
public:
  GomoryHuTree(const GraphData &graph, QWidget *parent = 0);

  void calc();
};

#endif
