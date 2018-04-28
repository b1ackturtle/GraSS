#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "initgraphdialog.h"
#include "graphicsview.h"

InitGraphDialog::InitGraphDialog(GraphicsView *view, QWidget *parent) :
  QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint)
{
  undirectedRadioButton = new QRadioButton("Undirected graph");
  undirectedRadioButton->setChecked(true);

  directedRadioButton = new QRadioButton("Directed graph");

  buttonBox = new QDialogButtonBox();
  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(undirectedRadioButton);
  mainLayout->addSpacerItem(new QSpacerItem(1, 10));
  mainLayout->addWidget(directedRadioButton);
  mainLayout->addWidget(buttonBox);

  setLayout(mainLayout);

  connect(undirectedRadioButton, SIGNAL(toggled(bool)), view, SLOT(setGraphType(bool)));

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), parent, SLOT(closeTab()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  setWindowTitle(tr("Select mode"));
}

void InitGraphDialog::keyPressEvent(QKeyEvent *e)
{
  if(e->key() != Qt::Key_Escape)
    QDialog::keyPressEvent(e);
}
