#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "insertgraph.h"
#include "graphicsview.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"

InsertGraph::InsertGraph(GraphicsView *view, QWidget *parent)
  : QUndoCommand(0)
{
  m_view = view;
  m_parent = parent;
}

void InsertGraph::undo()
{
  for(int i = 0; i < nodes.count(); i++) {
    Q_FOREACH(EdgeItem *edge, nodes[i]->edgeList()) {
      if(edge->scene() != NULL) {
	edge->nodeU()->removeAllEdgeList(edge);
	edge->nodeV()->removeAllEdgeList(edge);
	pll nodePair = GraphicsView::nodeItemPair(edge->nodeU(), edge->nodeV());
	(*m_view->multipleEdgeTable())[nodePair].removeAt(edge->multiTableIndex());
	m_view->scene()->removeItem(edge);
      }
    }
    m_view->scene()->removeItem(nodes[i]);
  }
}

void InsertGraph::redo()
{
  for(int i = 0; i < nodes.count(); i++) {
    m_view->scene()->addItem(nodes[i]);
  }
  for(int i = 0; i < edges.count(); i++) {
    edges[i]->setMultiTable(m_view->multipleEdgeTable());
    edges[i]->nodeU()->addEdgeItem(edges[i]);
    edges[i]->nodeV()->addEdgeItem(edges[i]);

    pll nodePair = GraphicsView::nodeItemPair(edges[i]->nodeU(), edges[i]->nodeV());
    (*m_view->multipleEdgeTable())[nodePair].append(edges[i]);

    m_view->scene()->addItem(edges[i]);
  }
}
