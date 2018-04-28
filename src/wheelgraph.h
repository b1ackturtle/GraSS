#ifndef WHEELGRAPH_H
#define WHEELGRAPH_H

#include <QDialog>
#include "insertgraph.h"

class GraphicsView;
class QLabel;
class QLineEdit;
class QDialogButtonBox;

class WheelGraph : public InsertGraph
{
public:
  WheelGraph(GraphicsView *view, QWidget *parent = 0);

  void run(long long int N, long long int R);
};

class WheelGraphDialog : public QDialog
{
  Q_OBJECT

public:
  WheelGraphDialog(QWidget *parent = 0);
  QLineEdit *wheelCountLineEdit;
  QLineEdit *radiusLineEdit;

private slots:
  void enableOkButton();

private:
  void createWidgets();
  void createLayouts();
  void createConnects();

  QLabel *wheelCountLabel;
  QLabel *radiusLabel;
  QDialogButtonBox *buttonBox;
};

#endif
