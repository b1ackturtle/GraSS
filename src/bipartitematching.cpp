#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "bipartitematching.h"
#include "graphicsview.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"

#include <vector>
#include <iostream>

BipartiteMatching::BipartiteMatching(const GraphData &graph, QWidget *parent)
  : Calc(graph, parent)
{
  tabTitle = tr("Bipartite Matching Result");
}

void BipartiteMatching::calc()
{
  GraphicsView *newView = createTab(true);

  graph = graphData->listGraph();
  N = graphData->nodeSize();
  match = std::vector< long long int >(N, -1);
  used = std::vector< bool >(N);

  long long int res = 0;
  for(long long int v = 0; v < N; v++) {
    if(match[v] < 0) {
      std::fill(used.begin(), used.end(), 0);
      if(dfs(v)) {
	res++;
      }
    }
  }

  Q_FOREACH(QGraphicsItem *i, newView->scene()->items()) {
    if(i->type() == EdgeItemType) {
      EdgeItem *edge = static_cast<EdgeItem*>(i);
      long long int id1 = graphData->smallNodeMapedID[edge->nodeU()->id()];
      long long int id2 = match[id1];
      if(id2 == graphData->smallNodeMapedID[edge->nodeV()->id()]) {
	edge->setColor(Qt::red);
      }
    }
  }

  resultDialogShow(res);
}

bool BipartiteMatching::dfs(long long int v)
{
  used[v] = true;
  for(auto g : graph[v]) {
    long long int u = g.to;
    long long int w = match[u];
    if(w < 0 || (!used[w] && dfs(w))) {
      match[v] = u;
      match[u] = v;
      return true;
    }
  }
  
  return false;
}

void BipartiteMatching::resultDialogShow(double result)
{
  resultDialog = new QDialog(m_parent);
  resultDialog->setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);
  QVBoxLayout *mainLayout = new QVBoxLayout;
  QHBoxLayout *res = new QHBoxLayout;
  res->addWidget(new QLabel(tr("Result:")));
  res->addWidget(new QLabel(QString::number(result)));
  QHBoxLayout *btn = new QHBoxLayout;
  btn->addStretch();
  QPushButton *okButton = new QPushButton(tr("OK"));
  connect(okButton, SIGNAL(clicked()), resultDialog, SLOT(accept()));
  btn->addWidget(okButton);
  btn->addStretch();
  mainLayout->addLayout(res);
  mainLayout->addLayout(btn);
  resultDialog->setLayout(mainLayout);
  resultDialog->show();
}
