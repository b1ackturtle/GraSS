#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "layoutdialog.h"

LayoutSelectDialog::LayoutSelectDialog(QWidget *parent)
  : QDialog(parent)
{
  sld = new SpringLayoutDialog(this);
  gld = new GeneticAlgorithmLayoutDialog(this);

  coulombParamLineEdit = sld->coulombParamLineEdit;
  bounceParamLineEdit = sld->bounceParamLineEdit;
  animationCheckBox = sld->animationCheckBox;
  genParamLineEdit = gld->genParamLineEdit;
  popParamLineEdit = gld->popParamLineEdit;
  eliteRateLineEdit = gld->eliteRateLineEdit;
  crossRateLineEdit = gld->crossRateLineEdit;
  mutateRateLineEdit = gld->mutateRateLineEdit;

  createWidgets();
  createLayouts();
  createConnections();

  setVisibleWidgets(0);
}

void LayoutSelectDialog::createWidgets()
{
  selectComboBox = new QComboBox();
  selectComboBox->addItem("Random Layout");
  selectComboBox->addItem("Spring Layout");
  selectComboBox->addItem("Genetic Algorithm Layout");

  buttonBox = new QDialogButtonBox();
  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);
}

void LayoutSelectDialog::createLayouts()
{
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(selectComboBox);
  mainLayout->addLayout(sld->mainLayout);
  mainLayout->addLayout(gld->mainLayout);
  mainLayout->addWidget(buttonBox);

  setLayout(mainLayout);
}

void LayoutSelectDialog::createConnections()
{
  connect(selectComboBox, SIGNAL(currentIndexChanged(int)),
	  this, SLOT(setVisibleWidgets(int)));

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void LayoutSelectDialog::setVisibleWidgets(int index)
{
  switch(index)
    {
    case 0:
      sld->setVisibleWidgets(false);
      gld->setVisibleWidgets(false);
      adjustSize();

      break;
    case 1:
      sld->setVisibleWidgets(true);
      gld->setVisibleWidgets(false);
      adjustSize();

      break;
    case 2:
      sld->setVisibleWidgets(false);
      gld->setVisibleWidgets(true);
      adjustSize();

      break;
    }
  adjustSize();
}

SpringLayoutDialog::SpringLayoutDialog(QWidget *parent)
  : QDialog(parent)
{
  m_parent = parent;

  normalCoulomb = 1000;
  normalBounce = 0.01;

  createWidgets();
  createLayouts();
  createConnections();
}

void SpringLayoutDialog::setVisibleWidgets(bool flag)
{
  coulombParamLabel->setVisible(flag);
  coulombParamLineEdit->setVisible(flag);
  bounceParamLabel->setVisible(flag);
  bounceParamLineEdit->setVisible(flag);
  animationCheckLabel->setVisible(flag);
  animationCheckBox->setVisible(flag);
}
 
void SpringLayoutDialog::createWidgets()
{
  coulombParamLabel = new QLabel(tr("Coulomb parameter:"));
  coulombParamLineEdit = new QLineEdit(QString::number(normalCoulomb));
  coulombParamLabel->setBuddy(coulombParamLineEdit);

  bounceParamLabel = new QLabel(tr("Bounce parameter:"));
  bounceParamLineEdit = new QLineEdit(QString::number(normalBounce));
  bounceParamLabel->setBuddy(bounceParamLineEdit);

  animationCheckLabel = new QLabel(tr("Animation:"));
  animationCheckBox = new QCheckBox;
  animationCheckLabel->setBuddy(animationCheckBox);

  if(m_parent->metaObject()->className() != QString("LayoutSelectDialog")) {
    buttonBox = new QDialogButtonBox();
    buttonBox->addButton(QDialogButtonBox::Ok);
    buttonBox->addButton(QDialogButtonBox::Cancel);
  }
}

void SpringLayoutDialog::createLayouts()
{
  QGridLayout *gridLayout = new QGridLayout;

  gridLayout->addWidget(coulombParamLabel, 0, 0, Qt::AlignLeft);
  gridLayout->addWidget(coulombParamLineEdit, 0, 1, Qt::AlignLeft);
  gridLayout->addWidget(bounceParamLabel, 1, 0, Qt::AlignLeft);
  gridLayout->addWidget(bounceParamLineEdit, 1, 1, Qt::AlignLeft);
  gridLayout->addWidget(animationCheckLabel, 2, 0, Qt::AlignLeft);
  gridLayout->addWidget(animationCheckBox, 2, 1, Qt::AlignLeft);

  mainLayout = new QVBoxLayout;
  mainLayout->addLayout(gridLayout);

  if(m_parent->metaObject()->className() != QString("LayoutSelectDialog")) {
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
  }
}

void SpringLayoutDialog::createConnections()
{
  if(m_parent->metaObject()->className() != QString("LayoutSelectDialog")) {
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  }
}

GeneticAlgorithmLayoutDialog::GeneticAlgorithmLayoutDialog(QWidget *parent)
  : QDialog(parent)
{
  m_parent = parent;

  normalGen = 1000;
  normalPop = 100;
  normalEliteRate = 0.3;
  normalCrossRate = 0.3;
  normalMutateRate = 0.1;

  createWidgets();
  createLayouts();
  createConnections();
}

void GeneticAlgorithmLayoutDialog::setVisibleWidgets(bool flag)
{
  genParamLabel->setVisible(flag);
  genParamLineEdit->setVisible(flag);
  popParamLabel->setVisible(flag);
  popParamLineEdit->setVisible(flag);
  eliteRateLabel->setVisible(flag);
  eliteRateLineEdit->setVisible(flag);
  crossRateLabel->setVisible(flag);
  crossRateLineEdit->setVisible(flag);
  mutateRateLabel->setVisible(flag);
  mutateRateLineEdit->setVisible(flag);
}

void GeneticAlgorithmLayoutDialog::createWidgets()
{
  genParamLabel = new QLabel("Generation:");
  genParamLineEdit = new QLineEdit(QString::number(normalGen));
  genParamLabel->setBuddy(genParamLineEdit);

  popParamLabel = new QLabel("Population:");
  popParamLineEdit = new QLineEdit(QString::number(normalPop));
  popParamLabel->setBuddy(popParamLineEdit);

  eliteRateLabel = new QLabel("Elite rate:");
  eliteRateLineEdit = new QLineEdit(QString::number(normalEliteRate));
  eliteRateLabel->setBuddy(eliteRateLineEdit);

  crossRateLabel = new QLabel("Crossover rate:");
  crossRateLineEdit = new QLineEdit(QString::number(normalCrossRate));
  crossRateLabel->setBuddy(crossRateLineEdit);

  mutateRateLabel = new QLabel("Mutate rate:");
  mutateRateLineEdit = new QLineEdit(QString::number(normalMutateRate));
  mutateRateLabel->setBuddy(mutateRateLineEdit);

  if(m_parent->metaObject()->className() != QString("LayoutSelectDialog")) {
    buttonBox = new QDialogButtonBox();
    buttonBox->addButton(QDialogButtonBox::Ok);
    buttonBox->addButton(QDialogButtonBox::Cancel);
  }
}

void GeneticAlgorithmLayoutDialog::createLayouts()
{
  QGridLayout *gridLayout = new QGridLayout;
  
  gridLayout->addWidget(genParamLabel, 0, 0, Qt::AlignLeft);
  gridLayout->addWidget(genParamLineEdit, 0, 1, Qt::AlignLeft);
  gridLayout->addWidget(popParamLabel, 1, 0, Qt::AlignLeft);
  gridLayout->addWidget(popParamLineEdit, 1, 1, Qt::AlignLeft);
  gridLayout->addWidget(eliteRateLabel, 2, 0, Qt::AlignLeft);
  gridLayout->addWidget(eliteRateLineEdit, 2, 1, Qt::AlignLeft);
  gridLayout->addWidget(crossRateLabel, 3, 0, Qt::AlignLeft);
  gridLayout->addWidget(crossRateLineEdit, 3, 1, Qt::AlignLeft);
  gridLayout->addWidget(mutateRateLabel, 4, 0, Qt::AlignLeft);
  gridLayout->addWidget(mutateRateLineEdit, 4, 1, Qt::AlignLeft);

  mainLayout = new QVBoxLayout;
  mainLayout->addLayout(gridLayout);

  if(m_parent->metaObject()->className() != QString("LayoutSelectDialog")) {
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
  }
}

void GeneticAlgorithmLayoutDialog::createConnections()
{
  if(m_parent->metaObject()->className() != QString("LayoutSelectDialog")) {
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  }
}
