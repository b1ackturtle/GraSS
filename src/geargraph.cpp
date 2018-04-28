#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "geargraph.h"
#include "graphicsview.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"

GearGraph::GearGraph(GraphicsView *view, QWidget *parent)
  : InsertGraph(view, parent)
{
}

void GearGraph::run(long long int N, long long int R)
{
  m_view->scene()->clearSelection();

  const int X = 0, Y = -R;
  const QPoint center = QPoint(m_view->scene()->width()/2, m_view->scene()->height()/2);

  NodeItem *node = new NodeItem(center, m_view->nodeId++);
  nodes.append(node);
  node->setSelected(true);

  node = new NodeItem(QPoint(X, Y) + center, m_view->nodeId++);
  nodes.append(node);
  node->setSelected(true);
  for(int i = 1; i < N; i++) {
    double theta = (2 * M_PI / N) * i;
    
    int x = X * cos(theta) - Y * sin(theta);
    int y = X * sin(theta) + Y * cos(theta);

    NodeItem *prev = nodes.back();
    node = new NodeItem((prev->position() + QPoint(x, y) + center)/2.0,
    			m_view->nodeId++);
    nodes.append(node);
    node->setSelected(true);

    node = new NodeItem(QPoint(x, y) + center, m_view->nodeId++);
    nodes.append(node);
    node->setSelected(true);
  }

  NodeItem *prev = nodes.back();
  node = new NodeItem((nodes[1]->position() + prev->position())/2.0,
  		      m_view->nodeId++);
  nodes.append(node);
  node->setSelected(true);

  EdgeItem *edge;
  for(int i = 1; i < nodes.count() - 1; i++) {
    edge = new EdgeItem(nodes[i]->position(), nodes[i+1]->position(),
  			nodes[i], nodes[i+1], m_view->graphType());
    edges.append(edge);
  }
  edge = new EdgeItem(nodes[1]->position(), nodes.back()->position(),
  		      nodes[1], nodes.back(), m_view->graphType());
  edges.append(edge);
  for(int i = 1; i < nodes.count(); i += 2) {
    edge = new EdgeItem(nodes[i]->position(), nodes.front()->position(),
  			nodes[i], nodes.front(), m_view->graphType());
    edges.append(edge);
  }
}

GearGraphDialog::GearGraphDialog(QWidget *parent)
  : QDialog(parent)
{
  createWidgets();
  createLayouts();
  createConnects();
}

void GearGraphDialog::enableOkButton()
{
  if(!gearCountLineEdit->text().isEmpty() && !radiusLineEdit->text().isEmpty()) {
    if(gearCountLineEdit->text().toLongLong() < 3)
      buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    else
      buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
  }
  else
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void GearGraphDialog::createWidgets()
{
  QRegExp rx("[0-9]{1,128}");

  gearCountLabel = new QLabel("G<sub>n</sub>:");
  gearCountLineEdit = new QLineEdit;
  gearCountLineEdit->setValidator(new QRegExpValidator(rx, this));
  gearCountLabel->setBuddy(gearCountLineEdit);

  radiusLabel = new QLabel(tr("Radius:"));
  radiusLineEdit = new QLineEdit;
  radiusLineEdit->setValidator(new QRegExpValidator(rx, this));
  radiusLabel->setBuddy(radiusLineEdit);

  buttonBox = new QDialogButtonBox;
  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void GearGraphDialog::createLayouts()
{
  QGridLayout *gridLayout = new QGridLayout;
  gridLayout->addWidget(gearCountLabel, 0, 0);
  gridLayout->addWidget(gearCountLineEdit, 0, 1);
  gridLayout->addWidget(radiusLabel, 1, 0);
  gridLayout->addWidget(radiusLineEdit, 1, 1);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(gridLayout);
  mainLayout->addWidget(buttonBox);

  setLayout(mainLayout);
}

void GearGraphDialog::createConnects()
{
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  connect(gearCountLineEdit, SIGNAL(textEdited(const QString)),
	  this, SLOT(enableOkButton()));
  connect(radiusLineEdit, SIGNAL(textEdited(const QString)),
	  this, SLOT(enableOkButton()));
}
