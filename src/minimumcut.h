#ifndef MINIMUMCUT_H
#define MINIMUMCUT_H

#include "maximumflow.h"
#include "graphdata.hpp"

class QLabel;
class QComboBox;
class QDialogButtonBox;

class MinimumCut : public MaximumFlow
{
  Q_OBJECT

public:
  MinimumCut(const GraphData &graph, QWidget *parent = 0);

  void calc();

private:
  void bfs(long long int s);
  double dfs(long long int v, long long int t, double f);
  void visitable(long long int v);

  std::vector< std::vector< Path > > G;
  std::vector< int > level;
  std::vector< int > iter;

  std::vector< bool > isStartSet;
};

#endif
