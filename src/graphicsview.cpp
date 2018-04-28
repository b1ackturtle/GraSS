#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "graphicsview.h"
#include "propertydialog.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"
#include "commands.h"

#include <iostream>

GraphicsView::GraphicsView(QGraphicsScene *scene, QWidget *parent)
  : QGraphicsView(scene)
  , m_graphType(UNDIRECTED)
  , isPrevSelectedItem(false)
  , prevItem(NULL)
  , m_toolFlag(tNode)
  , m_parent(parent)
  , dialog(NULL)
{
  setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  scene->setBackgroundBrush(QBrush(Qt::white, Qt::SolidPattern));

  m_multipleEdgeTable = new MultiEdgeHash;
  m_multipleLoopTable = new MultiLoopHash;

  nodeId = 0;

  m_undoStack = new QUndoStack(this);
}

GraphicsView::GraphicsView(const GraphicsView &g, QGraphicsScene *scn, QWidget *parent)
  : QGraphicsView(scn)
  , m_graphType(UNDIRECTED)
  , isPrevSelectedItem(false)
  , prevItem(NULL)
  , m_toolFlag(tNode)
  , m_parent(parent)
  , dialog(NULL)
{
  setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  scn->setBackgroundBrush(QBrush(Qt::white, Qt::SolidPattern));

  m_multipleEdgeTable = new MultiEdgeHash;
  m_multipleLoopTable = new MultiLoopHash;

  nodeId = g.nodeId;

  m_undoStack = new QUndoStack(this);

  setGraphType(g.graphType() == UNDIRECTED);
  std::vector<NodeItem*> t(nodeId + 1, NULL);
  Q_FOREACH(QGraphicsItem *i, g.scene()->items()) {
    if(i->type() == NodeItemType) {
      NodeItem *oldNode = static_cast<NodeItem*>(i);
      NodeItem *newNode;
      LoopItem *newLoop;
      if(oldNode->edgeList().length() == 0) {
	newNode = new NodeItem(*oldNode);
	scene()->addItem(newNode);
	Q_FOREACH(LoopItem *l, oldNode->loopList()) {
	  newLoop = new LoopItem(*l, newNode);
	  newNode->addLoopItem(newLoop);
	  scene()->addItem(newLoop);
	  multipleLoopTableAppend(newNode->id(), newLoop);
	  newLoop->setMultiTable(multipleLoopTable());
	}
      }
    }
    else if(i->type() == EdgeItemType) {
      NodeItem *newNodeU, *newNodeV;
      EdgeItem *newEdge, *oldEdge;
      LoopItem *newLoop;
      bool flagU = false, flagV = false;
      oldEdge = static_cast<EdgeItem*>(i);

      if(t[oldEdge->nodeU()->id()] == NULL) {
	newNodeU = new NodeItem(*(oldEdge->nodeU()));
	t[oldEdge->nodeU()->id()] = newNodeU;
	flagU = true;
      } else newNodeU = t[oldEdge->nodeU()->id()];
      if(t[oldEdge->nodeV()->id()] == NULL) {
	newNodeV = new NodeItem(*(oldEdge->nodeV()));
	t[oldEdge->nodeV()->id()] = newNodeV;
	flagV = true;
      } else newNodeV = t[oldEdge->nodeV()->id()];

      newEdge = new EdgeItem(*oldEdge, newNodeU, newNodeV);
      pll nodePair = nodeItemPair(newEdge->nodeU(), newEdge->nodeV());
      multipleEdgeTableAppend(nodePair, newEdge);
      newEdge->setMultiTable(multipleEdgeTable());

      newNodeU->addEdgeItem(newEdge);
      newNodeV->addEdgeItem(newEdge);

      if(flagU) {
	scene()->addItem(newNodeU);
	Q_FOREACH(LoopItem *l, oldEdge->nodeU()->loopList()) {
	  newLoop = new LoopItem(*l, newNodeU);
	  newNodeU->addLoopItem(newLoop);
	  scene()->addItem(newLoop);
	  multipleLoopTableAppend(newNodeU->id(), newLoop);
	  newLoop->setMultiTable(multipleLoopTable());
	}
      }
      if(flagV) {
	scene()->addItem(newNodeV);
	Q_FOREACH(LoopItem *l, oldEdge->nodeV()->loopList()) {
	  newLoop = new LoopItem(*l, newNodeV);
	  newNodeV->addLoopItem(newLoop);
	  scene()->addItem(newLoop);
	  multipleLoopTableAppend(newNodeV->id(), newLoop);
	  newLoop->setMultiTable(multipleLoopTable());
	}
      }
      scene()->addItem(newEdge);
    }
  }
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
  if(toolFlag() != tInfo) QGraphicsView::mousePressEvent(event);

  QPoint point = QPoint(event->x(), event->y());
  QGraphicsItem *item = scene()->itemAt(mapToScene(point), QTransform());
  isItem = (item == NULL) ? false : true;

  if(isItem) item->setFlag(QGraphicsItem::ItemIsMovable, false);

  if(!isItem) {
    scene()->clearSelection();
    scene()->update();
    update();
  }

  if(event->button() == Qt::LeftButton) {
    if(event->modifiers() == Qt::ControlModifier) {
      QGraphicsView::mousePressEvent(event);
      return;
    }

    if(toolFlag() == tNode && !isItem && !isPrevSelectedItem) {
      QPoint point(event->x(), event->y());
      NodeItem *node = new NodeItem(mapToScene(point), nodeId++);
      QUndoCommand *addNodeCommand = new AddNodeCommand(node, this);
      m_undoStack->push(addNodeCommand);

      emit modifiedScene(true);
    }
    else if(toolFlag() == tEdge && isItem && isPrevSelectedItem) {
      if(item->type() == NodeItemType && prevItem->type() == NodeItemType) {
	QPointF u = item->mapToScene(item->boundingRect().center());
	QPointF v = prevItem->mapToScene(prevItem->boundingRect().center());
	NodeItem* nodeItem = static_cast<NodeItem*>(item);
	NodeItem* nodePrevItem = static_cast<NodeItem*>(prevItem);

	if(item == prevItem) {
	  LoopItem *loop = new LoopItem(u, nodeItem, m_graphType);
	  QUndoCommand *addLoopCommand = new AddLoopCommand(loop, this);
	  m_undoStack->push(addLoopCommand);
	}
	else {
	  EdgeItem *edge = new EdgeItem(u, v, nodeItem, nodePrevItem, m_graphType);
	  QUndoCommand *addEdgeCommand = new AddEdgeCommand(edge, this);
	  m_undoStack->push(addEdgeCommand);
	}
      }

      emit modifiedScene(true);
    }
    else if(toolFlag() == tDelete && isItem && scene()) {
      QList< QGraphicsItem* > items;
      items.append(item);
      QUndoCommand *deleteCommand = new DeleteCommand(items, this);
      m_undoStack->push(deleteCommand);
      emit modifiedScene(true);
    }
    else if(toolFlag() == tMove && isItem) {
      m_oldPos.clear();
      if(item->type() == NodeItemType)  {
	m_oldPos.append(item->scenePos());
	Q_FOREACH(QGraphicsItem *i, scene()->selectedItems()) {
	  if(i->type() == NodeItemType) {
	    i->setFlag(QGraphicsItem::ItemIsMovable, true);
	    if(i == item) continue;
	    m_oldPos.append(i->scenePos());
	  }
	}
      }

      emit modifiedScene(true);
    }
    else if(toolFlag() == tInfo && isItem) {
      if(!item->isSelected()) {
	scene()->clearSelection();
	scene()->update();
	item->setSelected(true);
      }
      dialog = new PropertyDialog(scene()->selectedItems(), this);
      if(dialog->exec()) {
	QUndoCommand *infoCommand =
	  new InfoCommand(scene()->selectedItems(), dialog, this);
	m_undoStack->push(infoCommand);

	emit modifiedScene(true);
      }

      item->update();
    }
  }

  prevItem = (scene()->selectedItems().at(0));
  isPrevSelectedItem = (scene()->selectedItems().size() != 0);

  update();
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
  if(event->modifiers() == Qt::ControlModifier) {
    QGraphicsView::mouseReleaseEvent(event);
    return;
  }
  QPoint point = QPoint(event->x(), event->y());
  QGraphicsItem *item = scene()->itemAt(mapToScene(point), QTransform());
  if(item && toolFlag() == tMove && item->type() == NodeItemType && !m_oldPos.empty() &&
     m_oldPos.front() != item->scenePos() && event->button() == Qt::LeftButton) {
    QList< NodeItem* > selectedNodes;
    Q_FOREACH(QGraphicsItem *i, scene()->selectedItems()) {
      if(i->type() == NodeItemType)
	selectedNodes.append(static_cast<NodeItem*>(i));
    }
    m_undoStack->push(new MoveCommand(selectedNodes, m_oldPos));
  }
  QGraphicsView::mouseReleaseEvent(event);
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

  double scaleFactor = std::pow(4.0 / 3.0, (-1 * event->delta() / 240.0));
  scale(scaleFactor, scaleFactor);
}

QPoint GraphicsView::screenPos(QGraphicsItem *item)
{
  QPointF scenePos = item->mapToScene(item->boundingRect().center());
  QPoint viewPos = mapFromScene(scenePos);
  return viewPos;
}

pll GraphicsView::nodeItemPair(NodeItem *n, NodeItem *m)
{
  pll res;
  if(n->id() > m->id()) res = pll(n->id(), m->id());
  else res = pll(m->id(), n->id());

  return res;
}
