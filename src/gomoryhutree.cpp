#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "gomoryhutree.h"
#include "graphicsview.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"
#include "springlayout.h"
#include "randomlayout.h"

#include <vector>
#include <queue>
#include <iostream>

GomoryHuTree::GomoryHuTree(const GraphData &graph, QWidget *parent)
  : Calc(graph, parent)
{
  tabTitle = tr("Gomory-Hu Tree Result");
}

void GomoryHuTree::calc()
{
  GraphicsView *newView = createTab(false);

  long long int N = graphData->nodeSize();
  std::vector< std::vector< Edge > > graph = graphData->listGraph();
  std::vector< std::vector< double > > capacity(N, std::vector< double >(N, 0));

  std::vector< double > p(N, 0);
  std::vector< int > prev;
  std::vector< double > w(N);

  for(int i = 0; i < N; i++) {
    for(auto e : graph[i]) {
      capacity[e.from][e.to] += e.cost;
    }
  }

  for(int s = 1; s < N; s++) {
    int t = p[s];
    std::vector< std::vector< double > > flow(N, std::vector< double >(N, 0));
    
    double total = 0;
    while(1) {
      std::queue< int > Q;
      Q.push(s);
      prev.assign(N, -1); prev[s] = s;
      while(!Q.empty() && prev[t] < 0) {
	int u = Q.front(); Q.pop();
	for(auto e : graph[u]) {
	  if(prev[e.to] < 0 && capacity[u][e.to] - flow[u][e.to] > 0) {
	    prev[e.to] = u;
	    Q.push(e.to);
	  }
	}
      }

      if(prev[t] < 0) break;
      double inc = std::numeric_limits<double>::infinity();
      for(int j = t; prev[j] != j; j = prev[j])
	inc = std::min(inc, capacity[prev[j]][j] - flow[prev[j]][j]);
      for(int j = t; prev[j] != j; j = prev[j]) {
	flow[prev[j]][j] += inc;
	flow[j][prev[j]] -= inc;
      }
      total += inc;
    }
    w[s] = total;
    for(int u = 0; u < N; u++) {
      if(u != s && prev[u] != -1 && p[u] == t)
	p[u] = s;
    }
    if(prev[p[t]] != -1) {
      p[s] = p[t];
      p[t] = s;
      w[s] = w[t];
      w[t] = total;
    }
  }

  std::vector< std::vector< double > > T(N, std::vector< double >(N, std::numeric_limits<double>::infinity()));
  std::vector< NodeItem* > existNode(N, NULL);
  for(int s = 0; s < N; s++) {
    T[s][s] = 0;
    if(s != p[s]) {
      T[s][p[s]] = T[p[s]][s] = w[s];
    }
  }

  NodeItem *node;
  EdgeItem *edge;
  for(int i = 0; i < N - 1; i++) {
    for(int j = i + 1; j < N; j++) {
      if(!isinf(T[i][j])) {
  	if(existNode[i] == NULL) {
  	  node = new NodeItem(QPointF(0, 0), graphData->bigNodeMapedID[i]);
	  newView->scene()->addItem(node);
	  existNode[i] = node;
  	}
	if(existNode[j] == NULL) {
	  node = new NodeItem(QPointF(0, 0), graphData->bigNodeMapedID[j]);
	  newView->scene()->addItem(node);
	  existNode[j] = node;
	}

	edge = new EdgeItem(existNode[i]->position(), existNode[j]->position(),
			    existNode[i], existNode[j], UNDIRECTED);
	edge->setMultiTable(newView->multipleEdgeTable());
	edge->nodeU()->addEdgeItem(edge);
	edge->nodeV()->addEdgeItem(edge);
	
	pll nodePair = GraphicsView::nodeItemPair(edge->nodeU(), edge->nodeV());
	(*newView->multipleEdgeTable())[nodePair].append(edge);

	edge->setValue(T[i][j]);
	newView->scene()->addItem(edge);
      }
    }
  }
  newView->nodeId = graphData->bigNodeMapedID[N-1] + 1;

  RandomLayout rl(newView, this);
  rl.run();
  SpringLayout sl(newView, m_parent);
  sl.run(500, 0.1, Qt::Unchecked);
}
