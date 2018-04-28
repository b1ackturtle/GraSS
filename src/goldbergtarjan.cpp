#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "goldbergtarjan.h"
#include "graphicsview.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"

#include <vector>
#include <queue>

GoldbergTarjan::GoldbergTarjan(const GraphData &graph, QWidget *parent)
  : MaximumFlow(graph, parent)
{
  tabTitle = tr("Goldberg Tarjan Result");

  createWidgets();
  createLayouts();
  createConnects();
}

void GoldbergTarjan::calc()
{
  GraphicsView *newView = createTab(true);

  long long int N = graphData->nodeSize();
  graph = graphData->listGraph();
  start = graphData->smallNodeMapedID[startID->currentText().toLong()];
  end = graphData->smallNodeMapedID[endID->currentText().toLong()];
  int cnt = 0;

  flow = std::vector< std::vector< double > >(N, std::vector< double >(N, 0));
  capacity = std::vector< std::vector< double > >(N, std::vector< double >(N, 0));

  for(int u = 0; u < N; u++) {
    for(auto e : graph[u]) {
      capacity[e.from][e.to] += e.cap;
    }
  }

  excess = std::vector< double >(N);
  excess[start] = std::numeric_limits<double>::infinity();
  d = std::vector< long long int >(N);
  
  globalRelabeling();
  
  std::vector< std::queue< long long int > > B(N); B[d[start]].push(start);
  
  for(int b = d[start]; b >= 0; ) {
    if(B[b].empty()) {--b; continue;}
    long long int v = B[b].front(); B[b].pop();
    if(excess[v] == 0 || v == end) continue;
    
    for(auto e : graph[v]) {
      long long int w = e.to;
      if(capacity[v][w] - flow[v][w] > 0 && d[v] == d[w] + 1) {
	push(v, w);
	if(excess[w] > 0 && w != end) B[d[w]].push(w);
      }
    }
    if(excess[v] == 0) continue;
    d[v] = N;
    for(auto e : graph[v]) {
      if(capacity[v][e.to] - flow[v][e.to] > 0)
	d[v] = std::min(d[v], d[e.to] + 1);
    }
    if(d[v] < N) B[b = d[v]].push(v);

    if(++cnt % N == 0) globalRelabeling();
  }

  resultDialogShow(excess[end]);
}

void GoldbergTarjan::globalRelabeling()
{
  std::queue< long long int > Q; Q.push(end);
  std::fill(d.begin(), d.end(), std::numeric_limits<double>::infinity());
  d[end] = 0;
  while(!Q.empty()) {
    long long int u = Q.front(); Q.pop();
    for(auto e : graph[u]) {
      if(capacity[e.to][u] - flow[e.to][u] > 0 && d[u] + 1 < d[e.to]) {
	Q.push(e.to);
	d[e.to] = d[u] + 1;
      }
    }
  }
}

void GoldbergTarjan::push(long long int u, long long int v)
{
  double delta = std::min(excess[u], capacity[u][v] - flow[u][v]);
  flow[u][v] += delta;
  flow[v][u] -= delta;
  excess[u] -= delta;
  excess[v] += delta;
}
