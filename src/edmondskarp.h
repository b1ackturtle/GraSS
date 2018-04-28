#ifndef EDMONDSKARP_H
#define EDMONDSKARP_H

#include "maximumflow.h"
#include "graphdata.hpp"

class EdmondsKarp : public MaximumFlow
{
  Q_OBJECT

public:
  EdmondsKarp(const GraphData &graph, QWidget *parent = 0);

  void calc();
};

#endif
