#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "kruskal.h"
#include "graphicsview.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"

#include <vector>
#include <queue>

Kruskal::Kruskal(const GraphData &graph, QWidget *parent)
  : Calc(graph, parent)
{
  tabTitle = tr("Kruskal Result");
}

void Kruskal::calc()
{
  GraphicsView *newView = createTab(true);

  graph = graphData->listGraph();
  long long int N = graphData->nodeSize();

  //Kruskal's Algorithm
  UnionFind uf(N);
  auto comp = [](Edge a, Edge b) {return a.cost > b.cost;};
  std::priority_queue< Edge, std::vector<Edge>, decltype(comp) > Q(comp);

  for(int u = 0; u < N; u++) {
    for(auto e : graph[u]) {
      if(u < e.to) Q.push(e);
    }
  }

  double total = 0.0;
  std::vector< Edge > F;
  while(F.size() < N - 1 && !Q.empty()) {
    Edge e = Q.top(); Q.pop();
    if(!uf.same(e.from, e.to)) {
      total += e.cost;
      uf.unite(e.from, e.to);
      F.push_back(e);
    }
  }
  
  //Draw Minimum Spanning Tree
  Q_FOREACH(QGraphicsItem *i, newView->scene()->items()) {
    if(i->type() == EdgeItemType) {
      EdgeItem *edge = static_cast<EdgeItem*>(i);
      edge->setColor(Qt::black);
      long long int id1 = edge->nodeU()->id(), id2 = edge->nodeV()->id();
      bool deleteFlag = true;
      for(auto it = F.begin(); it != F.end(); ++it) {
	long long int p1 = graphData->bigNodeMapedID[(*it).from];
	long long int p2 = graphData->bigNodeMapedID[(*it).to];
	if((id1 == p1 && id2 == p2) || (id1 == p2 && id2 == p1))
	  deleteFlag = false;
      }
      if(deleteFlag) {
	pll nodePair = GraphicsView::nodeItemPair(edge->nodeU(), edge->nodeV());
	(*newView->multipleEdgeTable())[nodePair].removeAt(edge->multiTableIndex());
	edge->nodeU()->removeAllEdgeList(edge);
	edge->nodeV()->removeAllEdgeList(edge);

	newView->scene()->removeItem(edge);
      }
    }
  }

  result = total;
  resultDialogShow();
}

void Kruskal::resultDialogShow()
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

UnionFind::UnionFind(int n)
{
  par = std::vector< int >(n);
  rnk = std::vector< int >(n);

  for(int i = 0; i < n; i++) {
    par[i] = i;
    rnk[i] = 0;
  }
}

void UnionFind::unite(int x, int y)
{
  x = find(x); y = find(y);
  if(x == y) return;

  if(rnk[x] < rnk[y]) {
    par[x] = y;
  } else {
    par[y] = x;
    if(rnk[x] == rnk[y]) rnk[x]++;
  }
}

int UnionFind::find(int x)
{
  if(par[x] == x) {
    return x;
  } else {
    return par[x] = find(par[x]);
  }
}

bool UnionFind::same(int x, int y)
{
  return find(x) == find(y);
}
