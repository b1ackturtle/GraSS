#ifndef DINIC_H
#define DINIC_H

#include "maximumflow.h"
#include "graphdata.hpp"

class Dinic : public MaximumFlow
{
  Q_OBJECT

public:
  Dinic(const GraphData &graph, QWidget *parent = 0);

  void calc();

private:
  void bfs(long long int s);
  double dfs(long long int v, long long int t, double f);

  std::vector< std::vector< Path > > G;
  std::vector< int > level;
  std::vector< int > iter;
};

#endif
