#ifndef GEARGRAPH_H
#define GEARGRAPH_H

#include <QDialog>
#include "insertgraph.h"

class GraphicsView;
class QLabel;
class QLineEdit;
class QDialogButtonBox;

class GearGraph : public InsertGraph
{
public:
  GearGraph(GraphicsView *view, QWidget *parent = 0);

  void run(long long int N, long long int R);
};

class GearGraphDialog : public QDialog
{
  Q_OBJECT

public:
  GearGraphDialog(QWidget *parent = 0);
  QLineEdit *gearCountLineEdit;
  QLineEdit *radiusLineEdit;

private slots:
  void enableOkButton();

private:
  void createWidgets();
  void createLayouts();
  void createConnects();

  QLabel *gearCountLabel;
  QLabel *radiusLabel;
  QDialogButtonBox *buttonBox;
};

#endif
