#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "completebipartite.h"
#include "graphicsview.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"

CompleteBipartite::CompleteBipartite(GraphicsView *view, QWidget *parent)
  : InsertGraph(view, parent)
{
}

void CompleteBipartite::run(long long int M, long long int N, bool axis,
			    long long int space, long long int interval)
{
  m_view->scene()->clearSelection();

  const QPoint center = QPoint(m_view->scene()->width()/2, m_view->scene()->height()/2);

  if(!axis) { //Vertical
    QPoint origin = center - QPoint((double)space/2, ((double)M/2 - 0.5) * interval);
    for(int i = 0; i < M; i++) {
      NodeItem *node = new NodeItem(QPoint(0, i * interval) + origin, m_view->nodeId++);
      nodes.append(node);
      node->setSelected(true);
    }
    origin = center - QPoint(-(double)space/2, ((double)N/2 - 0.5) * interval);
    for(int i = 0; i < N; i++) {
      NodeItem *node = new NodeItem(QPoint(0, i * interval) + origin, m_view->nodeId++);
      nodes.append(node);
      node->setSelected(true);
    }
  } else { //Horizontal
    QPoint origin = center - QPoint(((double)M/2 - 0.5) * interval, (double)space/2);
    for(int i = 0; i < M; i++) {
      NodeItem *node = new NodeItem(QPoint(i * interval, 0) + origin, m_view->nodeId++);
      nodes.append(node);
      node->setSelected(true);
    }
    origin = center - QPoint(((double)N/2 - 0.5) * interval, -(double)space/2);
    for(int i = 0; i < N; i++) {
      NodeItem *node = new NodeItem(QPoint(i * interval, 0) + origin, m_view->nodeId++);
      nodes.append(node);
      node->setSelected(true);
    }
  }

  for(long long int i = 0; i < M; i++) {
    for(long long int j = M; j < N+M; j++) {
      EdgeItem *edge = new EdgeItem(nodes[i]->position(), nodes[j]->position(),
				    nodes[i], nodes[j], m_view->graphType());
      edges.append(edge);
    }
  }
}

CompleteBipartiteDialog::CompleteBipartiteDialog(QWidget *parent)
  : QDialog(parent)
{
  createWidgets();
  createLayouts();
  createConnects();
}

void CompleteBipartiteDialog::enableOkButton()
{
  if(!node1CountLineEdit->text().isEmpty() && !node2CountLineEdit->text().isEmpty() &&
     !spaceLineEdit->text().isEmpty() && !intervalLineEdit->text().isEmpty())
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
  else
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void CompleteBipartiteDialog::createWidgets()
{
  QRegExp rx("[0-9]{1,128}");

  node1CountLabel = new QLabel(tr("Number of Node1:"));
  node1CountLineEdit = new QLineEdit;
  node1CountLineEdit->setValidator(new QRegExpValidator(rx, this));
  node1CountLabel->setBuddy(node1CountLineEdit);

  node2CountLabel = new QLabel(tr("Number of Node2:"));
  node2CountLineEdit = new QLineEdit;
  node2CountLineEdit->setValidator(new QRegExpValidator(rx, this));
  node2CountLabel->setBuddy(node2CountLineEdit);

  axisLabel = new QLabel(tr("Axis:"));
  axisComboBox = new QComboBox;
  axisComboBox->addItem(tr("Vertical"));
  axisComboBox->addItem(tr("Horizontal"));
  axisLabel->setBuddy(axisComboBox);

  spaceLabel = new QLabel(tr("Space:"));
  spaceLineEdit = new QLineEdit;
  spaceLineEdit->setValidator(new QRegExpValidator(rx, this));
  spaceLabel->setBuddy(spaceLineEdit);

  intervalLabel = new QLabel(tr("Interval:"));
  intervalLineEdit = new QLineEdit;
  intervalLineEdit->setValidator(new QRegExpValidator(rx, this));
  intervalLabel->setBuddy(intervalLineEdit);

  buttonBox = new QDialogButtonBox;
  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void CompleteBipartiteDialog::createLayouts()
{
  QGridLayout *gridLayout = new QGridLayout;
  gridLayout->addWidget(node1CountLabel, 0, 0);
  gridLayout->addWidget(node1CountLineEdit, 0, 1);
  gridLayout->addWidget(node2CountLabel, 1, 0);
  gridLayout->addWidget(node2CountLineEdit, 1, 1);
  gridLayout->addWidget(axisLabel, 2, 0);
  gridLayout->addWidget(axisComboBox, 2, 1);
  gridLayout->addWidget(spaceLabel, 3, 0);
  gridLayout->addWidget(spaceLineEdit, 3, 1);
  gridLayout->addWidget(intervalLabel, 4, 0);
  gridLayout->addWidget(intervalLineEdit, 4, 1);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(gridLayout);
  mainLayout->addWidget(buttonBox);

  setLayout(mainLayout);
}

void CompleteBipartiteDialog::createConnects()
{
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  connect(node1CountLineEdit, SIGNAL(textEdited(const QString)),
	  this, SLOT(enableOkButton()));
  connect(node2CountLineEdit, SIGNAL(textEdited(const QString)),
	  this, SLOT(enableOkButton()));
  connect(spaceLineEdit, SIGNAL(textEdited(const QString)),
	  this, SLOT(enableOkButton()));
  connect(intervalLineEdit, SIGNAL(textEdited(const QString)),
	  this, SLOT(enableOkButton()));
}
