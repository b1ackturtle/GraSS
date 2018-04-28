#ifndef STARGRAPH_H
#define STARGRAPH_H

#include <QDialog>
#include "insertgraph.h"

class GraphicsView;
class QLabel;
class QLineEdit;
class QDialogButtonBox;
class NodeItem;
class EdgeItem;

class StarGraph : public InsertGraph
{
public:
  StarGraph(GraphicsView *view, QWidget *parent = 0);
  
  void run(long long int N, long long int R);
};

class StarGraphDialog : public QDialog
{
  Q_OBJECT

public:
  StarGraphDialog(QWidget *parent = 0);
  
  QLineEdit *nodeCountLineEdit;
  QLineEdit *radiusLineEdit;

private slots:
  void enableOkButton();

private:
  void createWidgets();
  void createLayouts();
  void createConnects();

  QLabel *nodeCountLabel;
  QLabel *radiusLabel;
  QDialogButtonBox *buttonBox;
};

#endif
