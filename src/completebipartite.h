#ifndef COMPLETEBIPARTITE_H
#define COMPLETEBIPARTITE_H

#include <QDialog>
#include "insertgraph.h"

class GraphicsView;
class QLabel;
class QLineEdit;
class QComboBox;
class QDialogButtonBox;
class NodeItem;
class EdgeItem;

class CompleteBipartite : public InsertGraph
{
public:
  CompleteBipartite(GraphicsView *view, QWidget *parent = 0);

  void run(long long int M, long long int N, bool axis,
	   long long int space, long long int interval);
};

class CompleteBipartiteDialog : public QDialog
{
  Q_OBJECT

public:
  CompleteBipartiteDialog(QWidget *parent = 0);
  QLineEdit *node1CountLineEdit;
  QLineEdit *node2CountLineEdit;
  QComboBox *axisComboBox;
  QLineEdit *spaceLineEdit;
  QLineEdit *intervalLineEdit;

private slots:
  void enableOkButton();

private:
  void createWidgets();
  void createLayouts();
  void createConnects();

  QLabel *node1CountLabel;
  QLabel *node2CountLabel;
  QLabel *axisLabel;
  QLabel *spaceLabel;
  QLabel *intervalLabel;
  QDialogButtonBox *buttonBox;
};

#endif
