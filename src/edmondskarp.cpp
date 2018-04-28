#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "edmondskarp.h"
#include "graphicsview.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"

#include <vector>
#include <queue>

EdmondsKarp::EdmondsKarp(const GraphData &graph, QWidget *parent)
  : MaximumFlow(graph, parent)
{
  tabTitle = tr("Edmonds Karp Result");

  createWidgets();
  createLayouts();
  createConnects();
}

void EdmondsKarp::calc()
{
  GraphicsView *newView = createTab(true);

  int N = graphData->nodeSize();
  std::vector< std::vector< Edge> > graph = graphData->listGraph();
  long long int start = graphData->smallNodeMapedID[startID->currentText().toLong()];
  long long int end = graphData->smallNodeMapedID[endID->currentText().toLong()];

  std::vector< std::vector< double > > flow(N, std::vector< double >(N));
  std::vector< std::vector< double > > capacity(N, std::vector< double >(N, 0));
  for(int u = 0; u < N; u++) {
    for(auto e : graph[u]) {
      capacity[e.from][e.to] += e.cap;
    }
  }

  double total = 0;
  while(1) {
    std::queue< long long int > Q;
    Q.push(start);

    std::vector< long long int > prev(N, -1);
    prev[start] = start;

    while(!Q.empty() && prev[end] < 0) {
      long long int q = Q.front(); Q.pop();
      for(auto e : graph[q]) {
	if(prev[e.to] < 0 && (capacity[q][e.to] - flow[q][e.to]) > 0) {
	  prev[e.to] = q;
	  Q.push(e.to);
	}
      }
    }
    
    if(prev[end] < 0) break;
    double res = std::numeric_limits<double>::infinity();
    for(long long int j = end; prev[j] != j; j = prev[j])
      res = std::min(res, (capacity[prev[j]][j] - flow[prev[j]][j]));
    for(long long int j = end; prev[j] != j; j = prev[j]) {
      flow[prev[j]][j] += res;
      flow[j][prev[j]] -= res;
    }
    total += res;
  }

  Q_FOREACH(QGraphicsItem *i, newView->scene()->items()) {
    if(i->type() == EdgeItemType) {
      EdgeItem *edge = static_cast<EdgeItem*>(i);
      long long int id1 = graphData->smallNodeMapedID[edge->nodeU()->id()];
      long long int id2 = graphData->smallNodeMapedID[edge->nodeV()->id()];
      edge->setFlow(std::abs(flow[id1][id2]));
    }
  }

  resultDialogShow(total);
}
