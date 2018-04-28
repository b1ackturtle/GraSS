#ifndef INDUCEDSUBGRAPH_H
#define INDUCEDSUBGRAPH_H

#include "calc.h"
#include "graphdata.hpp"

class QLabel;
class QLineEdit;

class InducedSubgraph : public Calc
{
  Q_OBJECT

public:
  InducedSubgraph(const GraphData &graph, QWidget *parent = 0);

private slots:
  void calc();
  void enableOkButton();

private:
  void createWidgets();
  void createLayouts();
  void createConnects();

  std::vector< std::string > split(const std::string &input, char delimiter);

  std::vector< bool > deleteFlag;

  QLabel *setNodesLabel;
  QLineEdit *setNodesLineEdit;
  QDialogButtonBox *buttonBox;
};

#endif
