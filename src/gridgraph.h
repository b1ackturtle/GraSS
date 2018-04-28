#ifndef GRIDGRAPH_H
#define GRIDGRAPH_H

#include <QDialog>
#include "insertgraph.h"

class GraphicsView;
class QLabel;
class QLineEdit;
class QDialogButtonBox;

class GridGraph : public InsertGraph
{
public:
  GridGraph(GraphicsView *view, QWidget *parent = 0);

  void run(long long int W, long long int H, long long int interval);
};

class GridGraphDialog : public QDialog
{
  Q_OBJECT

public:
  GridGraphDialog(QWidget *parent = 0);

  QLineEdit *heightLineEdit;
  QLineEdit *widthLineEdit;
  QLineEdit *intervalLineEdit;			  

private slots:
  void enableOkButton();

private:
  void createWidgets();
  void createLayouts();
  void createConnects();

  QLabel *heightLabel;
  QLabel *widthLabel;
  QLabel *intervalLabel;
  QDialogButtonBox *buttonBox;
};

#endif
