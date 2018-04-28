#ifndef TSPGA_H
#define TSPGA_H

#include "calc.h"
#include "graphdata.hpp"

class QComboBox;
class QCheckBox;

class TspGA : public Calc
{
  Q_OBJECT

public:
  TspGA(const GraphData &graph, QWidget *parent = 0);
  
  void calc();

private:
  void createWidgets();
  void createLayouts();
  void createConnects();

  double genetic(long long int popnum, int popsize, double mutprob, double elite,
		 int mixture, std::vector< long long int > &path);
  std::vector< long long int > mutate(std::vector< long long int > vec, int popnum);
  std::vector< long long int > crossover(std::vector< long long int > r1,
					 std::vector< long long int > r2, int n);
  double costf(const std::vector< long long int > &vec);

  void resultDialogShow(double result, std::vector< long long int > &path);

  std::vector< std::vector< double > > graph;
  int N;
  long long int start;

  QLabel *startIDLabel;
  QComboBox *startID;
  QLabel *cycleCheckLabel;
  QCheckBox *cycleCheckBox;

  QDialog *resultDialog;
  QDialogButtonBox *buttonBox;
};

#endif
