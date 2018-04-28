#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <queue>

#include "graphdata.hpp"
#include "graphicsview.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"

GraphData::GraphData(GraphicsView *view)
{
  QList<QGraphicsItem*> itemList = view->scene()->items();
  m_graphType = view->graphType();

  QList<NodeItem*> nodeList;
  QList<EdgeItem*> edgeList;
  QList<LoopItem*> loopList;

  Q_FOREACH(QGraphicsItem* item, itemList) {
    if(item->type() == NodeItemType)
      nodeList.append(static_cast<NodeItem*>(item));
    else if(item->type() == EdgeItemType)
      edgeList.append(static_cast<EdgeItem*>(item));
    else if(item->type() == LoopItemType)
      loopList.append(static_cast<LoopItem*>(item));
  }

  N = nodeList.size();
  int maxN = view->nodeId;
  existID = std::vector<bool>(maxN, false);
  
  Q_FOREACH(NodeItem *node, nodeList) {
    existID[node->id()] = true;
  }

  smallNodeMapedID = std::vector<long long int>(maxN, -1);
  bigNodeMapedID = std::vector<long long int>(maxN, -1);
  int newID = 0;
  for(int oldID = 0; oldID < maxN; oldID++) {
    if(existID[oldID]) {
      bigNodeMapedID[newID] = oldID;
      smallNodeMapedID[oldID] = newID++;
    }
  }
  bigNodeMapedID.resize(newID);

  m_matrixGraph = std::vector< std::vector<double> >(N, std::vector<double>(N, std::numeric_limits<double>::infinity()));

  for(int i = 0; i < N; i++)
    m_matrixGraph[i][i] = 0;

  flagNan = false; flagNeg = false;
  Q_FOREACH(EdgeItem* edge, edgeList) {
    long long int src = smallNodeMapedID[edge->nodeV()->id()];
    long long int dst = smallNodeMapedID[edge->nodeU()->id()];
    m_matrixGraph[src][dst] = std::min(edge->value(), m_matrixGraph[src][dst]);
    if(m_graphType == UNDIRECTED)
      m_matrixGraph[dst][src] = std::min(edge->value(), m_matrixGraph[src][dst]);
    if(m_matrixGraph[src][dst] < 0) flagNeg = true;
    if(isnan(m_matrixGraph[src][dst])) flagNan = true;
  }

  Q_FOREACH(LoopItem* loop, loopList) {
    int t = smallNodeMapedID[loop->node()->id()];
    m_matrixGraph[t][t] = loop->value();
  }

  m_listGraph = std::vector< std::vector<Edge> >(N);
  
  flagNanCap = false; flagNegCap = false;
  flagNanFlow = false; flagNegFlow = false;
  Q_FOREACH(EdgeItem *edge, edgeList) {
    long long int src = smallNodeMapedID[edge->nodeV()->id()];
    long long int dst = smallNodeMapedID[edge->nodeU()->id()];
    m_listGraph[src].push_back(Edge(src, dst, edge->value(), edge->capacity()));
    if(m_graphType == UNDIRECTED)
      m_listGraph[dst].push_back(Edge(dst, src, edge->value(), edge->capacity()));
    if(edge->capacity() < 0) flagNegCap = true;
    if(isnan(edge->capacity())) flagNanCap = true;
    if(edge->flow() < 0) flagNegFlow = true;
    if(isnan(edge->flow())) flagNanFlow = true;
  }
  Q_FOREACH(LoopItem *loop, loopList) {
    long long int src = smallNodeMapedID[loop->node()->id()];
    m_listGraph[src].push_back(Edge(src, src, loop->value()));
  }
}

GraphData::GraphData(long long int nodeSize, const std::vector< Edge > &edges) {
  m_graphType = UNDIRECTED;

  N = nodeSize;
  
  m_matrixGraph = std::vector< std::vector< double > >(N, std::vector< double >(N, std::numeric_limits< double >::infinity()));

  for(int i = 0; i < N; i++)
    m_matrixGraph[i][i] = 0;

  flagNan = false; flagNeg = false;
  for(auto edge : edges) {
    m_matrixGraph[edge.from][edge.to] = edge.cost;
    m_matrixGraph[edge.to][edge.from] = edge.cost;
    if(m_matrixGraph[edge.from][edge.to] < 0) flagNeg = true;
    if(isnan(m_matrixGraph[edge.from][edge.to])) flagNan = true;
  }

  m_listGraph = std::vector< std::vector< Edge > >(N);
  
  for(auto edge : edges) {
    m_listGraph[edge.from].push_back(Edge(edge.from, edge.to, edge.cost));
    m_listGraph[edge.to].push_back(Edge(edge.to, edge.from, edge.cost));
  }
}

GraphData::~GraphData()
{
}

bool GraphData::isBipartite()
{
  color = std::vector< int >(N, 0);
  for(int i = 0; i < N; i++) {
    if(color[i] == 0) {
      if(!dfs(i, 1)) {
	return false;
      }
    }
  }

  return true;
}

bool GraphData::dfs(long long int v, int c)
{
  color[v] = c;
  for(auto g : m_listGraph[v]) {
    if(color[g.to] == c) return false;
    if(color[g.to] == 0 && !dfs(g.to, -c)) return false;
  }

  return true;
}

bool GraphData::isTree()
{
  prev = std::vector< int >(N, -1);

  return !isCycle(0, -1);
}

bool GraphData::isCycle(long long int c, long long int p)
{
  prev[c] = p;
  for(auto e : m_listGraph[c]) {
    if(e.to != p) {
      if(prev[e.to] != -1) {
	prev[e.to] = c;
	return true;
      }
      if(isCycle(e.to, c)) return true;
    }
  }

  return false;
}

bool GraphData::isConnected()
{
  for(int i = 0; i < N; i++) {
    std::queue< int > Q;
    std::vector< bool > visited(N, false);

    Q.push(i);
    while(!Q.empty()) {
      int v = Q.front(); Q.pop();
      visited[v] = true;
      for(auto edge : m_listGraph[v]) {
	if(!visited[edge.to])
	  Q.push(edge.to);
      }
    }

    for(auto v : visited) {
      if(!v) return false;
    }
  }

  return true;
}
