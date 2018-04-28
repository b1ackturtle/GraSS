#ifndef GOLDBERGTARJAN_H
#define GOLDBERGTARJAN_H

#include "maximumflow.h"
#include "graphdata.hpp"

class GoldbergTarjan : public MaximumFlow
{
  Q_OBJECT

public:
  GoldbergTarjan(const GraphData &graph, QWidget *parent = 0);

  void calc();

private:
  void globalRelabeling();
  void push(long long int u, long long int v);

  std::vector< std::vector< Edge > > graph;
  long long int start;
  long long int end;

  std::vector< std::vector< double > > flow;
  std::vector< std::vector< double > > capacity;

  std::vector< double > excess;
  std::vector< long long int > d;
};

#endif
