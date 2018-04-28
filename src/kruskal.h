#ifndef KRUSKAL_H
#define KRUSKAL_H

#include "calc.h"
#include "graphdata.hpp"

class Kruskal : public Calc
{
  Q_OBJECT

public:
  Kruskal(const GraphData &graph, QWidget *parent = 0);

  void calc();

private:
  void resultDialogShow();

  std::vector< std::vector< Edge > > graph;

  double result;

  QDialog *resultDialog;
  QDialogButtonBox *buttonBox;
};

struct UnionFind
{
  std::vector< int > par;
  std::vector< int > rnk;

  UnionFind(int n);

  void unite(int x, int y);
  int find(int x);
  bool same(int x, int y);
};

#endif
