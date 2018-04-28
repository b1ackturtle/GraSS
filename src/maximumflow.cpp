#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "maximumflow.h"

MaximumFlow::MaximumFlow(const GraphData &graph, QWidget *parent)
  : Calc(graph, parent)
{
}

void MaximumFlow::enableOkButton()
{
  if(startID->currentText().isEmpty() || endID->currentText().isEmpty())
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  else if(startID->currentText() == endID->currentText())
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  else if(!graphData->existID[startID->currentText().toLong()] ||
	  !graphData->existID[endID->currentText().toLong()])
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  else
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void MaximumFlow::createWidgets()
{
  startIDLabel = new QLabel(tr("Start ID:"));
  startID = new QComboBox;
  startID->setMaximumWidth(50);
  startIDLabel->setBuddy(startID);

  endIDLabel = new QLabel(tr("End ID:"));
  endID = new QComboBox;
  endID->setMaximumWidth(50);
  endIDLabel->setBuddy(endID);

  for(unsigned int i = 0; i < graphData->existID.size(); i++) {
    if(graphData->existID[i]) {
      QString idx = QString::number(i);
      startID->addItem(idx);
      endID->addItem(idx);
    }
  }

  buttonBox = new QDialogButtonBox;
  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void MaximumFlow::createLayouts()
{
  QHBoxLayout *idLayout = new QHBoxLayout;
  idLayout->addWidget(startIDLabel);
  idLayout->addWidget(startID);
  idLayout->addSpacerItem(new QSpacerItem(10, 1));
  idLayout->addWidget(endIDLabel);
  idLayout->addWidget(endID);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(idLayout);
  mainLayout->addWidget(buttonBox);

  setLayout(mainLayout);
}

void MaximumFlow::createConnects()
{
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(calc()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  connect(startID, SIGNAL(currentIndexChanged(const QString &)),
	  this, SLOT(enableOkButton()));
  connect(endID, SIGNAL(currentIndexChanged(const QString &)),
	  this, SLOT(enableOkButton()));
}

void MaximumFlow::resultDialogShow(double result)
{
  resultDialog = new QDialog(m_parent);
  resultDialog->setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);
  QVBoxLayout *mainLayout = new QVBoxLayout;
  QHBoxLayout *start = new QHBoxLayout;
  start->addWidget(startIDLabel);
  start->addWidget(new QLabel(startID->currentText()));
  QHBoxLayout*end = new QHBoxLayout;
  end->addWidget(endIDLabel);
  end->addWidget(new QLabel(endID->currentText()));
  QHBoxLayout *res = new QHBoxLayout;
  res->addWidget(new QLabel(tr("Result:")));
  res->addWidget(new QLabel(QString::number(result)));
  QHBoxLayout *btn = new QHBoxLayout;
  btn->addStretch();
  QPushButton *okButton = new QPushButton(tr("OK"));
  connect(okButton, SIGNAL(clicked()), resultDialog, SLOT(accept()));
  btn->addWidget(okButton);
  btn->addStretch();
  mainLayout->addLayout(start);
  mainLayout->addLayout(end);
  mainLayout->addLayout(res);
  mainLayout->addLayout(btn);
  resultDialog->setLayout(mainLayout);
  resultDialog->show();
}
