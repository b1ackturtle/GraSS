#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "minimumcostflow.h"
#include "graphicsview.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"

#include <vector>
#include <algorithm>

MinimumCostFlow::MinimumCostFlow(const GraphData &graph, QWidget *parent)
  : Calc(graph, parent)
{
  tabTitle = tr("Minimum Cost Flow Result");

  createWidgets();
  createLayouts();
  createConnects();
}

void MinimumCostFlow::calc()
{
  GraphicsView *newView = createTab(true);

  std::vector< std::vector< Edge > > edges = graphData->listGraph();
  long long int N = graphData->nodeSize();
  long long int start = graphData->smallNodeMapedID[startID->currentText().toLongLong()];
  long long int end = graphData->smallNodeMapedID[endID->currentText().toLongLong()];
  double flow = flowLineEdit->text().toDouble();

  std::vector< std::vector< Path > > G(N);
  for(auto edge : edges) {
    for(auto e : edge) {
      G[e.from].push_back(Path(e.to, e.cap, e.cost, (double)G[e.to].size()));
      G[e.to].push_back(Path(e.from, 0, -e.cost, (double)G[e.from].size() - 1));
    }
  }

  double result = 0;
  std::vector< int > prevV(N), prevE(N);
  while(flow > 0) {
    std::vector< double > dist(N, std::numeric_limits<double>::infinity());
    dist[start] = 0;
    bool update = true;
    while(update) {
      update = false;
      for(int v = 0; v < N; v++) {
	if(dist[v] == std::numeric_limits<double>::infinity()) continue;
	for(int i = 0; i < G[v].size(); i++) {
	  Path &p = G[v][i];
	  if(p.cap > 0 && dist[p.to] > dist[v] + p.cost) {
	    dist[p.to] = dist[v] + p.cost;
	    prevV[p.to] = v;
	    prevE[p.to] = i;
	    update = true;
	  }
	}
      }
    }

    if(dist[end] == std::numeric_limits<double>::infinity()) {
      result = -1;
      break;
    }
  
    double d = flow;
    for(int v = end; v != start; v = prevV[v]) {
      d = std::min(d, G[prevV[v]][prevE[v]].cap);
    }
    flow -= d;
    result += d * dist[end];
    for(int v = end; v != start; v = prevV[v]) {
      Path &p = G[prevV[v]][prevE[v]];
      p.cap -= d;
      G[v][p.rev].cap += d;
    }
  }

  Q_FOREACH(QGraphicsItem *i, newView->scene()->items()) {
    if(i->type() == EdgeItemType) {
      EdgeItem *edge = static_cast<EdgeItem*>(i);
      long long int id1 = graphData->smallNodeMapedID[edge->nodeU()->id()];
      long long int id2 = graphData->smallNodeMapedID[edge->nodeV()->id()];
      for(auto g : G[id1]) {
	if(g.to == id2) edge->setFlow(g.cap);
      }
    }
  }

  resultDialogShow(result);
}

void MinimumCostFlow::enableOkButton()
{
  if(startID->currentText().isEmpty() || endID->currentText().isEmpty())
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  else if(startID->currentText() == endID->currentText())
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  else if(flowLineEdit->text().isEmpty())
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  else
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void MinimumCostFlow::createWidgets()
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

  flowLabel = new QLabel(tr("Flow:"));
  flowLineEdit = new QLineEdit;
  QRegExp rx("[0-9]{1,128}");
  flowLineEdit->setValidator(new QRegExpValidator(rx, this));
  flowLabel->setBuddy(flowLineEdit);

  buttonBox = new QDialogButtonBox;
  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void MinimumCostFlow::createLayouts()
{
  QHBoxLayout *idLayout = new QHBoxLayout;
  idLayout->addWidget(startIDLabel);
  idLayout->addWidget(startID);
  idLayout->addSpacerItem(new QSpacerItem(10, 1));
  idLayout->addWidget(endIDLabel);
  idLayout->addWidget(endID);

  QHBoxLayout *flowLayout = new QHBoxLayout;
  flowLayout->addWidget(flowLabel);
  flowLayout->addWidget(flowLineEdit);
  
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(idLayout);
  mainLayout->addLayout(flowLayout);
  mainLayout->addWidget(buttonBox);

  setLayout(mainLayout);
}

void MinimumCostFlow::createConnects()
{
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(calc()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  connect(startID, SIGNAL(currentIndexChanged(const QString &)),
	  this, SLOT(enableOkButton()));
  connect(endID, SIGNAL(currentIndexChanged(const QString &)),
	  this, SLOT(enableOkButton()));
  connect(flowLineEdit, SIGNAL(textEdited(const QString)),
	  this, SLOT(enableOkButton()));
}

void MinimumCostFlow::resultDialogShow(double result)
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
