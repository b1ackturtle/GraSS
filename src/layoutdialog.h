#ifndef LAYOUTDIALOG_H
#define LAYOUTDIALOG_H

#include <QDialog>

class QComboBox;
class QLineEdit;
class QCheckBox;
class QVBoxLayout;
class QLabel;
class QDialogButtonBox;

class SpringLayoutDialog : public QDialog
{
public:
  SpringLayoutDialog(QWidget *parent = 0);

  void setVisibleWidgets(bool flag);

  QLineEdit *coulombParamLineEdit;
  QLineEdit *bounceParamLineEdit;
  QCheckBox *animationCheckBox;

  QVBoxLayout *mainLayout;

private:
  void createWidgets();
  void createLayouts();
  void createConnections();

  QWidget *m_parent;

  double normalCoulomb;
  double normalBounce;

  QLabel *coulombParamLabel;
  QLabel *bounceParamLabel;
  QLabel *animationCheckLabel;

  QDialogButtonBox *buttonBox;
};

class GeneticAlgorithmLayoutDialog : public QDialog
{
public:
  GeneticAlgorithmLayoutDialog(QWidget *parent = 0);

  void setVisibleWidgets(bool flag);

  QLineEdit *genParamLineEdit;
  QLineEdit *popParamLineEdit;
  QLineEdit *eliteRateLineEdit;
  QLineEdit *crossRateLineEdit;
  QLineEdit *mutateRateLineEdit;

  QVBoxLayout *mainLayout;

private:
  void createWidgets();
  void createLayouts();
  void createConnections();
  
  QWidget *m_parent;

  long long int normalGen;
  long long int normalPop;
  double normalEliteRate;
  double normalCrossRate;
  double normalMutateRate;

  QLabel *genParamLabel;
  QLabel *popParamLabel;
  QLabel *eliteRateLabel;
  QLabel *crossRateLabel;
  QLabel *mutateRateLabel;

  QDialogButtonBox *buttonBox;
};

class LayoutSelectDialog : public QDialog
{
  Q_OBJECT
  
public:
  LayoutSelectDialog(QWidget *parent = 0);
  
  QComboBox *selectComboBox;
  QLineEdit *coulombParamLineEdit;
  QLineEdit *bounceParamLineEdit;
  QCheckBox *animationCheckBox;
  QLineEdit *genParamLineEdit;
  QLineEdit *popParamLineEdit;
  QLineEdit *eliteRateLineEdit;
  QLineEdit *crossRateLineEdit;
  QLineEdit *mutateRateLineEdit;

public slots:
  void setVisibleWidgets(int index);

private:
  void createWidgets();
  void createConnections();
  void createLayouts();

  SpringLayoutDialog *sld;
  GeneticAlgorithmLayoutDialog *gld;

  QDialogButtonBox *buttonBox;
};

#endif
