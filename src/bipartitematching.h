#ifndef BIPARTITEMATCHING_H
#define BIPARTITEMATCHING_H

#include "calc.h"
#include "graphdata.hpp"

class BipartiteMatching : public Calc
{
  Q_OBJECT

public:
  BipartiteMatching(const GraphData &graph, QWidget *parent = 0);

  void calc();
  void resultDialogShow(double result);

private:
  bool dfs(long long int v);

  std::vector< std::vector< Edge > > graph;
  long long int N;
  std::vector< long long int > match;
  std::vector< bool > used;

  QDialog *resultDialog;
};

#endif
