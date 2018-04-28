#ifndef HELDKARP_H
#define HELDKARP_H

#include "calc.h"
#include "graphdata.hpp"

class QComboBox;
class QCheckBox;

class HeldKarp : public Calc
{
  Q_OBJECT

public:
  HeldKarp(const GraphData &graph, QWidget *parent = 0);

  void calc();
  int maxN() const {return m_maxN;}

private:
  void createWidgets();
  void createLayouts();
  void createConnects();

  void resultDialogShow(double result, std::vector< long long int > &path);
  std::pair<double, long long int> bitDP(int S, int v);
  std::pair<double, long long int> dp[1 << 21][21];

  std::vector< std::vector< double > > graph;
  int N;
  long long int start;
  const int m_maxN;

  QLabel *startIDLabel;
  QComboBox *startID;
  QLabel *cycleCheckLabel;
  QCheckBox *cycleCheckBox;

  QDialog *resultDialog;
  QDialogButtonBox *buttonBox;
};

#endif
