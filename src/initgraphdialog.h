#ifndef INITGRAPHDIALOG_H
#define INITGRAPHDIALOG_H

#include <QDialog>

class QRadioButton;
class QDialogButtonBox;
class GraphicsView;

class InitGraphDialog : public QDialog
{
  Q_OBJECT

public:
  InitGraphDialog(GraphicsView *view, QWidget *parent = 0);

protected:
  void keyPressEvent(QKeyEvent *e);

private:
  QRadioButton *directedRadioButton;
  QRadioButton *undirectedRadioButton;
  QDialogButtonBox *buttonBox;
};

#endif
