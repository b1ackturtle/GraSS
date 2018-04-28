#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "stargraph.h"
#include "graphicsview.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"

StarGraph::StarGraph(GraphicsView *view, QWidget *parent)
  : InsertGraph(view, parent)
{
}

void StarGraph::run(long long int N, long long int R)
{
  m_view->scene()->clearSelection();

  const int X = 0, Y = -R;
  const QPoint center = QPoint(m_view->scene()->width()/2, m_view->scene()->height()/2);

  NodeItem *node = new NodeItem(center, m_view->nodeId++);
  node->setSelected(true);
  nodes.append(node);
  for(int i = 0; i < N - 1; i++) {
    double theta = (2*M_PI / (N - 1)) * i;

    int x = X * cos(theta) - Y * sin(theta);
    int y = X * sin(theta) + Y * cos(theta);

    NodeItem *node = new NodeItem(QPoint(x, y) + center,
				  m_view->nodeId++);
    nodes.append(node);
    node->setSelected(true);
  }

  for(int i = 1; i < nodes.count(); i++) {
    EdgeItem *edge = new EdgeItem(nodes[0]->position(), nodes[i]->position(),
				  nodes[0], nodes[i], m_view->graphType());
    edges.append(edge);
  }
}

StarGraphDialog::StarGraphDialog(QWidget *parent)
  : QDialog(parent)
{
  createWidgets();
  createLayouts();
  createConnects();
}

void StarGraphDialog::enableOkButton()
{
  if(!nodeCountLineEdit->text().isEmpty() && !radiusLineEdit->text().isEmpty()) {
    if(nodeCountLineEdit->text().toLongLong() <= 0)
      buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    else
      buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
  }
  else
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void StarGraphDialog::createWidgets()
{
  nodeCountLabel = new QLabel(tr("Number of nodes:"));
  nodeCountLineEdit = new QLineEdit;
  QRegExp rx("[0-9]{1,128}");
  nodeCountLineEdit->setValidator(new QRegExpValidator(rx, this));
  nodeCountLabel->setBuddy(nodeCountLineEdit);

  radiusLabel = new QLabel(tr("Radius:"));
  radiusLineEdit = new QLineEdit;
  radiusLineEdit->setValidator(new QRegExpValidator(rx, this));
  radiusLabel->setBuddy(radiusLineEdit);

  buttonBox = new QDialogButtonBox;
  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void StarGraphDialog::createLayouts()
{
  QGridLayout *gridLayout = new QGridLayout;
  gridLayout->addWidget(nodeCountLabel, 0, 0);
  gridLayout->addWidget(nodeCountLineEdit, 0, 1);
  gridLayout->addWidget(radiusLabel, 1, 0);
  gridLayout->addWidget(radiusLineEdit, 1, 1);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(gridLayout);
  mainLayout->addWidget(buttonBox);

  setLayout(mainLayout);
}

void StarGraphDialog::createConnects()
{
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  connect(nodeCountLineEdit, SIGNAL(textEdited(const QString)),
	  this, SLOT(enableOkButton()));
  connect(radiusLineEdit, SIGNAL(textEdited(const QString)),
	  this, SLOT(enableOkButton()));
}
