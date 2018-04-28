#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "heldkarp.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"
#include "graphicsview.h"

#include <vector>
#include <map>
#include <iostream>

HeldKarp::HeldKarp(const GraphData &graph, QWidget *parent)
  : Calc(graph, parent)
  , m_maxN(20)
{
  tabTitle = tr("Held-Karp Result");

  createWidgets();
  createLayouts();
  createConnects();
}

void HeldKarp::createWidgets()
{
  startIDLabel = new QLabel(tr("Start ID:"));
  startID = new QComboBox;
  startID->setMaximumWidth(50);
  startIDLabel->setBuddy(startID);

  for(unsigned int i = 0; i < graphData->existID.size(); i++) {
    if(graphData->existID[i]) {
      QString idx = QString::number(i);
      startID->addItem(idx);
    }
  }

  cycleCheckLabel = new QLabel(tr("Cycle:"));
  cycleCheckBox = new QCheckBox;

  buttonBox = new QDialogButtonBox;
  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);
}

void HeldKarp::createLayouts()
{
  QHBoxLayout *idLayout = new QHBoxLayout;
  idLayout->addWidget(startIDLabel);
  idLayout->addWidget(startID);

  idLayout->addWidget(cycleCheckLabel);
  idLayout->addWidget(cycleCheckBox);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(idLayout);
  mainLayout->addWidget(buttonBox);

  setLayout(mainLayout);
}

void HeldKarp::createConnects()
{
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(calc()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void HeldKarp::calc()
{
  GraphicsView *newView = createTab(true);

  graph = graphData->matrixGraph();
  N = graphData->nodeSize();
  start = graphData->smallNodeMapedID[startID->currentText().toLong()];
  
  for(int S = 0; S < (1 << maxN()); S++) {
    for(int i = 0; i < maxN(); i++) {
      dp[S][i].first = -1;
    }
  }
  std::pair<double, long long int> res = 
    (cycleCheckBox->checkState()) ? bitDP(0, start) : bitDP(1 << start, start);

  std::vector<long long int> path;
  int S = (cycleCheckBox->checkState()) ? 0 : 1 << start;
  int p = start;
  while(S != (1 << N)-1) {
    int next = dp[S][p].second;
    S |= (1 << next);
    p = next;
    path.push_back(p);
  }

  std::vector< std::vector< bool > > pathTable(N, std::vector< bool >(N, false));
  pathTable[start][path[0]] = pathTable[path[0]][start] = true;
  for(int i = 0; i < path.size() - 1; i++) {
    pathTable[path[i]][path[i+1]] = pathTable[path[i+1]][path[i]] = true;
  }
  Q_FOREACH(QGraphicsItem *i, newView->scene()->items()) {
    if(i->type() == EdgeItemType) {
      EdgeItem *edge = static_cast<EdgeItem*>(i);
      long long int id1 = graphData->smallNodeMapedID[edge->nodeU()->id()];
      long long int id2 = graphData->smallNodeMapedID[edge->nodeV()->id()];
      if(pathTable[id1][id2])
	edge->setColor(Qt::red);
    }
  }
 
  resultDialogShow(res.first, path);
}

std::pair<double, long long int> HeldKarp::bitDP(int S, int v)
{
  if(dp[S][v].first >= 0) return dp[S][v];

  if(cycleCheckBox->checkState()) {
    if(S == (1 << N)-1 && v == start) return dp[S][v] = std::make_pair(0, -1);
  } else {
    if(S == (1 << N)-1) return dp[S][v] = std::make_pair(0, -1);
  }

  std::pair<double, long long int> res(std::numeric_limits<double>::infinity(), -1);
  for(int u = 0; u < N; u++) {
    if(!(S >> u & 1)) {
      std::pair<double, long long int> tmp = bitDP(S|1<<u, u);
      tmp.first += graph[v][u];
      if(tmp.first < res.first) {
	res.first = tmp.first;
	res.second = u;
      }
    }
  }

  return dp[S][v] = res;
}

void HeldKarp::resultDialogShow(double result, std::vector< long long int > &path)
{
  resultDialog = new QDialog(m_parent);
  resultDialog->setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  QHBoxLayout *res = new QHBoxLayout;
  res->addWidget(new QLabel(tr("Result:")));
  res->addWidget(new QLabel(QString::number(result)));
  QHBoxLayout *pathLayout = new QHBoxLayout;
  pathLayout->addWidget(new QLabel("Path:"));
  pathLayout->addWidget(new QLabel(QString::number(graphData->bigNodeMapedID[start])));
  for(auto p : path) {
    pathLayout->addWidget(new QLabel("--->"));
    pathLayout->addWidget(new QLabel(QString::number(graphData->bigNodeMapedID[p])));
  }
  QHBoxLayout *btn = new QHBoxLayout;
  btn->addStretch();
  QPushButton *okButton = new QPushButton(tr("OK"));
  connect(okButton, SIGNAL(clicked()), resultDialog, SLOT(accept()));
  btn->addWidget(okButton);
  btn->addStretch();
  mainLayout->addLayout(res);
  mainLayout->addLayout(pathLayout);
  mainLayout->addLayout(btn);
  resultDialog->setLayout(mainLayout);
  resultDialog->show();
}
