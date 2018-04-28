#ifndef CALC_H
#define CALC_H

#include <QDialog>

class MainWindow;
class GraphicsView;
class QDialogButtonBox;
class GraphData;

class Calc : public QDialog
{
  Q_OBJECT

public:
  Calc(const GraphData &graph, QWidget *parent = 0);

  GraphicsView *createTab(bool copyFlag);
  bool checkNan();
  bool checkNeg();
  bool checkNanCap();
  bool checkNegCap();
  bool checkNanFlow();
  bool checkNegFlow();
  
private slots:
  virtual void calc() {return;}

private:
  virtual void createWidgets() {return;}
  virtual void createLayouts() {return;}
  virtual void createConnects(){return;}

protected:
  const GraphData *graphData;
  MainWindow *resultWindow;
  QString tabTitle;
  QWidget *m_parent;
};

#endif
