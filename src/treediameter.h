#ifndef TREEDIAMETER_H
#define TREEDIAMETER_H

#include "calc.h"
#include "graphdata.hpp"

class TreeDiameter : public Calc
{
  Q_OBJECT

public:
  TreeDiameter(const GraphData &graph, QWidget *parent = 0);

  void calc();
  void resultDialogShow(const double result,
			const long long int x, const long long int y);

private:
  std::pair< double, long long int > dist(long long int u, long long int v);
  std::vector< long long int > path(long long int t, std::vector< long long int > p,
				    std::vector< bool > visited);

  std::vector< std::vector< Edge > > graph;
  long long int N;

  QDialog *resultDialog;
};

#endif
