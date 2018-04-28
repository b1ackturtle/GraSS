#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "dijkstra.h"
#include "graphicsview.h"
#include "graphdata.hpp"
#include "mainwindow.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"

#include <vector>
#include <unordered_map>
#include <iostream>
#include <queue>

Dijkstra::Dijkstra(const GraphData &graph, QWidget *parent)
  : Calc(graph, parent)
{
  tabTitle = tr("Dijkstra Result");

  createWidgets();
  createLayouts();
  createConnects();
}

void Dijkstra::calc()
{
  GraphicsView *newView = createTab(true);

  distList = graphData->listGraph();
  long long int N = graphData->nodeSize();
  long long int start = graphData->smallNodeMapedID[startID->currentText().toLong()];
  long long int end = graphData->smallNodeMapedID[endID->currentText().toLong()];
  prev = std::vector< long long int >(N, -1);

  //Dijkstra Algorithm
  std::vector<double> d(N, std::numeric_limits<double>::infinity());
  typedef std::pair<double, long long int> P;
  std::priority_queue<P, std::vector<P>, std::greater<P> > que;
  d[start] = 0.0;
  que.push(P(0.0, start));

  while(!que.empty()) {
    P p = que.top(); que.pop();
    int v = p.second;
    if(d[v] < p.first) continue;
    for(Edge e : distList[v]) {
      if(d[e.to] > d[v] + e.cost) {
	d[e.to] = d[v] + e.cost;
	que.push(P(d[e.to], e.to));
	prev[e.to] = v;
      }
    }
  }

  //Recover Path
  for(long long int pos = end; pos != -1; pos = prev[pos])
    path.push_back(pos);

  //Draw Minimum Path
  Q_FOREACH(QGraphicsItem *i, newView->scene()->items()) {
    if(i->type() == EdgeItemType) {
      EdgeItem *edge = static_cast<EdgeItem*>(i);
      edge->setColor(Qt::black);
      long long int id1 = edge->nodeU()->id(), id2 = edge->nodeV()->id();
      for(auto it = path.rbegin(); it != path.rend() - 1; ++it) {
	long long int path1 = graphData->bigNodeMapedID[*it];
	long long int path2 = graphData->bigNodeMapedID[*(it+1)];
	if((id1 == path1 && id2 == path2) || (id1 == path2 && id2 == path1))
	  edge->setColor(Qt::red);
      }
    }
    else if(i->type() == NodeItemType) static_cast<NodeItem*>(i)->setColor(Qt::black);
    else if(i->type() == LoopItemType) static_cast<LoopItem*>(i)->setColor(Qt::black);
  }

  result = d[end];
  resultDialogShow();
}

void Dijkstra::enableOkButton()
{
  if(startID->currentText().isEmpty() || endID->currentText().isEmpty())
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  else if(!graphData->existID[startID->currentText().toLong()] ||
	  !graphData->existID[endID->currentText().toLong()])
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  else
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void Dijkstra::createWidgets()
{
  startIDLabel = new QLabel(tr("Start ID:"));
  startID = new QComboBox;
  startID->setMaximumWidth(50);
  startIDLabel->setBuddy(startID);

  endIDLabel = new QLabel(tr("End ID:"));
  endID = new QComboBox;
  endID->setMaximumWidth(50);
  endIDLabel->setBuddy(endID);

  for(unsigned int i = 0; i < graphData->existID.size(); i++)
    if(graphData->existID[i]) {
      QString idx = QString::number(i);
      startID->addItem(idx);
      endID->addItem(idx);
    }

  buttonBox = new QDialogButtonBox;
  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void Dijkstra::createLayouts()
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

void Dijkstra::createConnects()
{
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(calc()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  connect(startID, SIGNAL(currentIndexChanged(const QString &)),
	  this, SLOT(enableOkButton()));
  connect(endID, SIGNAL(currentIndexChanged(const QString &)),
	  this, SLOT(enableOkButton()));
}

void Dijkstra::resultDialogShow()
{
  resultDialog = new QDialog(m_parent);
  resultDialog->setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);
  QVBoxLayout *mainLayout = new QVBoxLayout;
  QHBoxLayout *start = new QHBoxLayout;
  start->addWidget(startIDLabel);
  start->addWidget(new QLabel(startID->currentText()));
  QHBoxLayout *end = new QHBoxLayout;
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
