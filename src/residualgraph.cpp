#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "residualgraph.h"
#include "graphicsview.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"

ResidualGraph::ResidualGraph(const GraphData &graph, QWidget *parent)
  : Calc(graph, parent)
{
  tabTitle = tr("Residual Graph Result");
}

void ResidualGraph::calc()
{
  GraphicsView *newView = createTab(true);
  newView->setGraphType(false);

  Q_FOREACH(QGraphicsItem *item, newView->scene()->items()) {
    if(item->type() == EdgeItemType) {
      EdgeItem *edge = static_cast<EdgeItem*>(item);
      if(edge->flow() < edge->capacity())
	edge->setValue(edge->capacity() - edge->flow());
      else {
	pll nodePair = GraphicsView::nodeItemPair(edge->nodeU(), edge->nodeV());
	(*newView->multipleEdgeTable())[nodePair].removeAt(edge->multiTableIndex());
	edge->nodeU()->removeAllEdgeList(edge);
	edge->nodeV()->removeAllEdgeList(edge);
	newView->scene()->removeItem(edge);
	QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
      }
      if(edge->flow() > 0) {
      	EdgeItem *revEdge = new EdgeItem(edge->nodeV()->position() + edge->nodeV()->pos(),
      					 edge->nodeU()->position() + edge->nodeU()->pos(),
      					 edge->nodeV(), edge->nodeU(), DIRECTED);
      	revEdge->setValue(edge->flow());

      	revEdge->setMultiTable(newView->multipleEdgeTable());
      	revEdge->nodeU()->addEdgeItem(revEdge);
      	revEdge->nodeV()->addEdgeItem(revEdge);

      	pll nodePair = GraphicsView::nodeItemPair(revEdge->nodeU(), revEdge->nodeV());
      	(*newView->multipleEdgeTable())[nodePair].append(revEdge);

      	newView->scene()->addItem(revEdge);
	QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
      }

      edge->setEdgeType(DIRECTED);
      edge->setFlow(std::numeric_limits<double>::quiet_NaN());
      edge->setCapacity(std::numeric_limits< double >::quiet_NaN());
    }
  }
}
