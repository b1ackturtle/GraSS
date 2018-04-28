#ifndef TARJAN_H
#define TARJAN_H

#include "calc.h"
#include "graphdata.hpp"

class Tarjan : public Calc
{
  Q_OBJECT

public:
  Tarjan(const GraphData &graph, QWidget *parent = 0);

  void calc();

private:
  void dfs(int v);
  void rdfs(int v, int k);

  std::vector< std::vector< long long int > > G;
  std::vector< std::vector< long long int > > rG;
  long long int N;
  std::vector< long long int > vs;
  std::vector< bool > used;
  std::vector< long long int > cmp;
};

#endif
