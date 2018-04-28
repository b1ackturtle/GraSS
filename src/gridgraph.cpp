#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "gridgraph.h"
#include "graphicsview.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"

GridGraph::GridGraph(GraphicsView *view, QWidget *parent)
  : InsertGraph(view, parent)
{
}

void GridGraph::run(long long int W, long long int H, long long int interval)
{
  m_view->scene()->clearSelection();

  const QPoint center = QPoint(m_view->scene()->width()/2, m_view->scene()->height()/2);
  const QPoint origin =
    center - QPoint(((double)W/2 - 0.5) * interval, ((double)H/2 - 0.5) * interval);

  for(int y = 0; y < H; y++) {
    for(int x = 0; x < W; x++) {
      NodeItem *node = new NodeItem(QPoint(x * interval, y * interval) + origin,
				    m_view->nodeId++);
      nodes.append(node);
      node->setSelected(true);
    }
  }

  for(int i = 1; i < nodes.count(); i++) {
    if(i % W == 0) continue;
    EdgeItem *edge = new EdgeItem(nodes[i-1]->position(), nodes[i]->position(),
  				  nodes[i-1], nodes[i], m_view->graphType());
    edges.append(edge);
  }
  for(int i = 0; i < nodes.count() - W; i++) {
    EdgeItem *edge = new EdgeItem(nodes[i]->position(), nodes[i+W]->position(),
  				  nodes[i], nodes[i+W], m_view->graphType());
    edges.append(edge);
  }
}

GridGraphDialog::GridGraphDialog(QWidget *parent)
  : QDialog(parent)
{
  createWidgets();
  createLayouts();
  createConnects();
}

void GridGraphDialog::enableOkButton()
{
  if(!widthLineEdit->text().isEmpty() && !heightLineEdit->text().isEmpty() &&
     !intervalLineEdit->text().isEmpty()) {
    if(widthLineEdit->text().toLongLong() <= 0 || heightLineEdit->text().toLongLong() <=0)
      buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    else
      buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
  }
  else
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void GridGraphDialog::createWidgets()
{
  QRegExp rx("[0-9]{1,128}");

  widthLabel = new QLabel(tr("Width:"));
  widthLineEdit = new QLineEdit;
  widthLineEdit->setValidator(new QRegExpValidator(rx, this));
  widthLabel->setBuddy(widthLineEdit);

  heightLabel = new QLabel(tr("Height:"));
  heightLineEdit = new QLineEdit;
  heightLineEdit->setValidator(new QRegExpValidator(rx, this));
  heightLabel->setBuddy(heightLineEdit);

  intervalLabel = new QLabel(tr("Interval:"));
  intervalLineEdit = new QLineEdit;
  intervalLineEdit->setValidator(new QRegExpValidator(rx, this));
  intervalLabel->setBuddy(intervalLineEdit);

  buttonBox = new QDialogButtonBox;
  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void GridGraphDialog::createLayouts()
{
  QGridLayout *gridLayout = new QGridLayout;
  gridLayout->addWidget(widthLabel, 0, 0);
  gridLayout->addWidget(widthLineEdit, 0, 1);
  gridLayout->addWidget(heightLabel, 1, 0);
  gridLayout->addWidget(heightLineEdit, 1, 1);
  gridLayout->addWidget(intervalLabel, 2, 0);
  gridLayout->addWidget(intervalLineEdit, 2, 1);
  
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(gridLayout);
  mainLayout->addWidget(buttonBox);

  setLayout(mainLayout);
}

void GridGraphDialog::createConnects()
{
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  connect(widthLineEdit, SIGNAL(textEdited(const QString)),
	  this, SLOT(enableOkButton()));
  connect(heightLineEdit, SIGNAL(textEdited(const QString)),
	  this, SLOT(enableOkButton()));
  connect(intervalLineEdit, SIGNAL(textEdited(const QString)),
	  this, SLOT(enableOkButton()));
}
