#ifndef CONTRACTION_H
#define CONTRACTION_H

#include "calc.h"
#include "graphdata.hpp"

class QLabel;
class QLineEdit;

class Contraction : public Calc
{
  Q_OBJECT

public:
  Contraction(const GraphData &graph, QWidget *parent = 0);

private slots:
  void calc();
  void enableOkButton();

private:
  void createWidgets();
  void createLayouts();
  void createConnects();

  int contraction(int edge1, int edge2, GraphicsView *view);

  std::vector< std::string > split(const std::string &input, char delimiter);

  std::vector< std::vector< double > > selectableEdges;
  std::vector< std::pair< int, int > > deleteEdges;

  QLabel *setEdgesLabel;
  QLineEdit *setEdgesLineEdit;
  QDialogButtonBox *buttonBox;
};

#endif
