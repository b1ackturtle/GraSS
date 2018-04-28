#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "randomlayout.h"
#include "graphicsview.h"
#include "mainwindow.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"
#include "tools.hpp"

#include <vector>
#include <random>

RandomLayout::RandomLayout(GraphicsView *view, QWidget *parent)
  : QUndoCommand(0)
{
  m_view = view;
  m_parent = parent;
  runFlag = false;
}

RandomLayout::RandomLayout(GraphicsView *view, long long int n,
			   std::vector< Edge > edges, QWidget *parent)
  : QUndoCommand(0)
{
  m_view = view;
  m_parent = parent;
  m_edges = edges;
  N = n;
  runFlag = true;

  view->scene()->clear();
  view->nodeId = 0;
  view->multipleEdgeTableClear();
  view->multipleLoopTableClear();
}

void RandomLayout::run()
{
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> randomX(0, 1280);
  std::uniform_int_distribution<int> randomY(0, 720);

  if(runFlag) {
    std::vector<NodeItem*> nodes(N, NULL);
    NodeItem *node;
    EdgeItem *edge;
    LoopItem *loop;
    int rx, ry;
    QPointF randPos;

    for(long long int i = 0; i < N; i++) {
      rx = randomX(mt); ry = randomY(mt);
      randPos = m_view->mapToScene(QPoint(rx, ry));
      node = new NodeItem(randPos, i);
      m_view->nodeId++;
      nodes[i] = node;

      m_view->scene()->addItem(node);
    }

    for(Edge e : m_edges) {
      if(e.from != e.to) {
	NodeItem *nodeU = nodes[e.from];
	NodeItem *nodeV = nodes[e.to];

	edge = new EdgeItem(nodeU->position(), nodeV->position(), nodeU, nodeV,
			    m_view->graphType());
	edge->setValue(e.cost);

	nodeU->addEdgeItem(edge);
	nodeV->addEdgeItem(edge);
	edge->setMultiTable(m_view->multipleEdgeTable());
	pll nodePair = GraphicsView::nodeItemPair(nodeU, nodeV);
	(*m_view->multipleEdgeTable())[nodePair].append(edge);

	m_view->scene()->addItem(edge);
      } else {
	NodeItem *node = nodes[e.from];

	loop = new LoopItem(node->position(), node, m_view->graphType());
	loop->setValue(e.cost);

	node->addLoopItem(loop);
	loop->setMultiTable(m_view->multipleLoopTable());
	(*m_view->multipleLoopTable())[node->id()].append(loop);

	m_view->scene()->addItem(loop);
      }
    }
  } else {
    int rx, ry;
    QPointF randPos;
    NodeItem *node;

    oldPos = std::vector<QPointF>(m_view->nodeId);
    newPos = std::vector<QPointF>(m_view->nodeId);

    Q_FOREACH(QGraphicsItem *item, m_view->scene()->items()) {
      if(item->type() == NodeItemType) {
	node = static_cast<NodeItem*>(item);

	oldPos[node->id()] = node->pos();

	rx = randomX(mt); ry = randomY(mt);
	randPos = QPointF(rx, ry);
	node->setPos(randPos - node->position());

	newPos[node->id()] = node->pos();

	nodeList.push_back(node);
      }
    }
  }
}

void RandomLayout::undo()
{
  Q_FOREACH(NodeItem *node, nodeList) {
    node->setPos(oldPos[node->id()]);
  }
}

void RandomLayout::redo()
{
  Q_FOREACH(NodeItem *node, nodeList) {
    node->setPos(newPos[node->id()]);
  }
}
