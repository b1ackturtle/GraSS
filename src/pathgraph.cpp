#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "pathgraph.h"
#include "graphicsview.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"

PathGraph::PathGraph(GraphicsView *view, QWidget *parent)
  : InsertGraph(view, parent)
{
}

void PathGraph::run(long long int N, long long int interval, bool axis)
{
  m_view->scene()->clearSelection();

  const QPoint center = QPoint(m_view->scene()->width()/2, m_view->scene()->height()/2);
  
  if(!axis) { //Vertical
    const QPoint origin = center - QPoint(0, N/2 * interval);
    for(int i = 0; i < N; i++) {
      NodeItem *node = new NodeItem(QPoint(0, i * interval) + origin, m_view->nodeId++);
      nodes.append(node);
      node->setSelected(true);
    }
  } else { //Horizontal
    const QPoint origin = center - QPoint(N/2 * interval, 0);
    for(int i = 0; i < N; i++) {
      NodeItem *node = new NodeItem(QPoint(i * interval, 0) + origin, m_view->nodeId++);
      nodes.append(node);
      node->setSelected(true);
    }
  }

  for(long long int i = 0; i < N - 1; i++) {
    EdgeItem *edge = new EdgeItem(nodes[i]->position(), nodes[i+1]->position(),
				  nodes[i], nodes[i+1], m_view->graphType());
    edges.append(edge);
  }
}

PathGraphDialog::PathGraphDialog(QWidget *parent)
  : QDialog(parent)
{
  createWidgets();
  createLayouts();
  createConnects();
}

void PathGraphDialog::enableOkButton()
{
  if(!nodeCountLineEdit->text().isEmpty() && !intervalLineEdit->text().isEmpty()) {
    if(nodeCountLineEdit->text().toLongLong() <= 0)
      buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    else
      buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
  }
  else
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}
    
void PathGraphDialog::createWidgets()
{
  QRegExp rx("[0-9]{1,128}");

  nodeCountLabel = new QLabel(tr("Number of Node:"));
  nodeCountLineEdit = new QLineEdit;
  nodeCountLineEdit->setValidator(new QRegExpValidator(rx, this));
  nodeCountLabel->setBuddy(nodeCountLineEdit);

  intervalLabel = new QLabel(tr("Interval:"));
  intervalLineEdit = new QLineEdit;
  intervalLineEdit->setValidator(new QRegExpValidator(rx, this));
  intervalLabel->setBuddy(intervalLineEdit);

  axisLabel = new QLabel(tr("Axis:"));
  axisComboBox = new QComboBox;
  axisComboBox->addItem(tr("Vertical"));
  axisComboBox->addItem(tr("Horizontal"));
  axisLabel->setBuddy(axisComboBox);

  buttonBox = new QDialogButtonBox;
  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void PathGraphDialog::createLayouts()
{
  QGridLayout *gridLayout = new QGridLayout;
  gridLayout->addWidget(nodeCountLabel, 0, 0);
  gridLayout->addWidget(nodeCountLineEdit, 0, 1);
  gridLayout->addWidget(intervalLabel, 1, 0);
  gridLayout->addWidget(intervalLineEdit, 1, 1);
  gridLayout->addWidget(axisLabel, 2, 0);
  gridLayout->addWidget(axisComboBox, 2, 1);
  
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(gridLayout);
  mainLayout->addWidget(buttonBox);

  setLayout(mainLayout);
}

void PathGraphDialog::createConnects()
{
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  connect(nodeCountLineEdit, SIGNAL(textEdited(const QString)),
	  this, SLOT(enableOkButton()));
  connect(intervalLineEdit, SIGNAL(textEdited(const QString)),
	  this, SLOT(enableOkButton()));
}
