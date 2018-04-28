#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "dinic.h"
#include "graphicsview.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"

#include <vector>
#include <queue>

Dinic::Dinic(const GraphData &graph, QWidget *parent)
  : MaximumFlow(graph, parent)
{
  tabTitle = tr("Dinic Result");

  createWidgets();
  createLayouts();
  createConnects();
}

void Dinic::calc()
{
  GraphicsView *newView = createTab(true);

  long long int N = graphData->nodeSize();
  std::vector< std::vector< Edge > > edges = graphData->listGraph();
  G = std::vector< std::vector< Path > >(N);
  level = std::vector< int >(N);
  iter = std::vector< int >(N);

  for(auto edge : edges) {
    for(auto e : edge) {
      G[e.from].push_back(Path(e.to, e.cap, (double)G[e.to].size()));
      G[e.to].push_back(Path(e.from, 0, (double)G[e.from].size() - 1));
    }
  }

  long long int start = graphData->smallNodeMapedID[startID->currentText().toLong()];
  long long int end = graphData->smallNodeMapedID[endID->currentText().toLong()];
  double flow = 0;
  while(1) {
    bfs(start);
    if(level[end] < 0) break;
    std::fill(iter.begin(), iter.end(), 0);
    double f;
    while((f = dfs(start, end, std::numeric_limits<double>::infinity())) > 0) {
      flow += f;
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

  resultDialogShow(flow);
}

void Dinic::bfs(long long int s)
{
  std::fill(level.begin(), level.end(), -1);
  std::queue< long long int > Q;
  level[s] = 0;
  Q.push(s);
  while(!Q.empty()) {
    long long int v = Q.front(); Q.pop();
    for(int i = 0; i < G[v].size(); i++) {
      Path p = G[v][i];
      if(p.cap > 0 && level[p.to] < 0) {
	level[p.to] = level[v] + 1;
	Q.push(p.to);
      }
    }
  }
}

double Dinic::dfs(long long int v, long long int t, double f) {
  if(v == t) return f;
  for(int &i = iter[v]; i < G[v].size(); i++) {
    Path &p = G[v][i];
    if(p.cap > 0 && level[v] < level[p.to]) {
      double d = dfs(p.to, t, std::min(f, p.cap));
      if(d > 0) {
	p.cap -= d;
	G[p.to][p.rev].cap += d;
	return d;
      }
    }
  }
  return 0;
}
