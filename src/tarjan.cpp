#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "tarjan.h"
#include "graphicsview.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"

#include <vector>

Tarjan::Tarjan(const GraphData &graph, QWidget *parent)
  : Calc(graph, parent)
{
  tabTitle = tr("Tarjan's SCC Result");
}

void Tarjan::calc()
{
  GraphicsView *newView = createTab(true);

  std::vector< std::vector< Edge > > graph = graphData->listGraph();
  N = graphData->nodeSize();
  used = std::vector< bool >(N, false);
  cmp = std::vector< long long int >(N);

  //Tarjan's strongly connected component Algorithm
  G = std::vector< std::vector< long long int > >(N);
  rG = std::vector< std::vector< long long int > >(N);
  for(long long int i = 0; i < graph.size(); i++) {
    for(long long int j = 0; j < graph[i].size(); j++) {
      G[graph[i][j].from].push_back(graph[i][j].to);
      rG[graph[i][j].to].push_back(graph[i][j].from);
    }
  }

  vs.clear();
  for(int v = 0; v < N; v++) {
    if(!used[v]) dfs(v);
  }
  used = std::vector< bool >(N, false);
  int k = 0;
  for(int i = vs.size() - 1; i >= 0; i--) {
    if(!used[vs[i]]) rdfs(vs[i], k++);
  }

  //Draw Strongly Connected Components
  Q_FOREACH(QGraphicsItem *i, newView->scene()->items()) {
    if(i->type() == EdgeItemType) {
      EdgeItem *edge = static_cast<EdgeItem*>(i);
      edge->setColor(Qt::black);
      long long int id1 = edge->nodeU()->id(), id2 = edge->nodeV()->id();
      if(cmp[graphData->smallNodeMapedID[id1]] != cmp[graphData->smallNodeMapedID[id2]]) {
	pll nodePair = GraphicsView::nodeItemPair(edge->nodeU(), edge->nodeV());
	(*newView->multipleEdgeTable())[nodePair].removeAt(edge->multiTableIndex());
	newView->scene()->removeItem(edge);
      }
    }
  }
}

void Tarjan::dfs(int v)
{
  used[v] = true;
  for(int i = 0; i < G[v].size(); i++) {
    if(!used[G[v][i]]) dfs(G[v][i]);
  }
  vs.push_back(v);
}

void Tarjan::rdfs(int v, int k)
{
  used[v] = true;
  cmp[v] = k;
  for(int i = 0; i < rG[v].size(); i++) {
    if(!used[rG[v][i]]) rdfs(rG[v][i], k);
  }
}
