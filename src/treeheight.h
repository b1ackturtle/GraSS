#ifndef TREEHEIGHT_H
#define TREEHEIGHT_H

#include "calc.h"
#include "graphdata.hpp"

class TreeHeight : public Calc
{
  Q_OBJECT

public:
  TreeHeight(const GraphData &graph, QWidget *parent = 0);

  void calc();
  void resultDialogShow(const std::vector< double > ht);

private:
  std::vector< double > height();
  void dist(long long int u, std::vector< double > &vec);

  std::vector< std::vector< Edge > > graph;
  long long int N;

  QDialog *resultDialog;
};

#endif
