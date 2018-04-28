#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "commands.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"
#include "graphicsview.h"
#include "propertydialog.h"

#include <QDebug>

AddNodeCommand::AddNodeCommand(NodeItem *node, GraphicsView *view, QUndoCommand *parent)
  : QUndoCommand(parent)
{
  m_node = node;
  m_scene = view->scene();
  m_view = view;

  m_scene->update();
}

AddNodeCommand::~AddNodeCommand()
{
  if(!m_node->scene())
    delete m_node;
}

void AddNodeCommand::undo()
{
  Q_FOREACH(EdgeItem *e, m_node->edgeList()) {
    if(e->scene() != NULL)
      m_scene->removeItem(e);
  }

  m_scene->removeItem(m_node);
  m_scene->update();
}

void AddNodeCommand::redo()
{
  m_scene->addItem(m_node);
  m_scene->update();
}

AddEdgeCommand::AddEdgeCommand(EdgeItem *edge, GraphicsView *view, QUndoCommand *parent)
  : QUndoCommand(parent)
{
  m_edge = edge;
  m_scene = view->scene();
  m_view = view;

  m_scene->update();
}

AddEdgeCommand::~AddEdgeCommand()
{
  if(!m_edge->scene())
    delete m_edge;
}

void AddEdgeCommand::undo()
{
  pll nodePair = GraphicsView::nodeItemPair(m_edge->nodeU(), m_edge->nodeV());
  (*m_view->multipleEdgeTable())[nodePair].removeAt(m_edge->multiTableIndex());
  m_edge->nodeU()->removeAllEdgeList(m_edge);
  m_edge->nodeV()->removeAllEdgeList(m_edge);
  m_scene->removeItem(m_edge);
  m_scene->update();
}

void AddEdgeCommand::redo()
{
  m_edge->setMultiTable(m_view->multipleEdgeTable());

  m_edge->nodeU()->addEdgeItem(m_edge);
  m_edge->nodeV()->addEdgeItem(m_edge);

  pll nodePair = GraphicsView::nodeItemPair(m_edge->nodeU(), m_edge->nodeV());
  (*m_view->multipleEdgeTable())[nodePair].append(m_edge);

  m_scene->addItem(m_edge);
  m_scene->update();
}

AddLoopCommand::AddLoopCommand(LoopItem *loop, GraphicsView *view, QUndoCommand *parent)
  : QUndoCommand(parent)
{
  m_loop = loop;
  m_scene = view->scene();
  m_view = view;

  m_scene->update();
}

AddLoopCommand::~AddLoopCommand()
{
  if(!m_loop->scene())
    delete m_loop;
}

void AddLoopCommand::undo()
{
  (*m_view->multipleLoopTable())[m_loop->node()->id()].removeAt(m_loop->multiTableIndex());
  m_loop->node()->removeAllLoopList(m_loop);

  m_scene->removeItem(m_loop);
  m_scene->update();
}

void AddLoopCommand::redo()
{
  m_loop->setMultiTable(m_view->multipleLoopTable());
  m_loop->node()->addLoopItem(m_loop);

  (*m_view->multipleLoopTable())[m_loop->node()->id()].append(m_loop);

  m_scene->addItem(m_loop);
  m_scene->update();
}

MoveCommand::MoveCommand(const QList< NodeItem* > &nodes, const QList< QPointF > &oldPos,
			 QUndoCommand *parent)
  : QUndoCommand(parent)
{
  m_nodes = nodes;
  m_oldPos = oldPos;
  Q_FOREACH(NodeItem *node, nodes) {
    newPos.append(node->pos());
  }
}

bool MoveCommand::mergeWith(const QUndoCommand *command)
{
  const MoveCommand *moveCommand = static_cast<const MoveCommand *>(command);
  QList< NodeItem* > nodes = moveCommand->m_nodes;

  if(m_nodes != nodes) return false;

  newPos.clear();
  Q_FOREACH(NodeItem *node, nodes) {
    newPos.append(node->pos());
  }
  
  return true;
}

void MoveCommand::undo()
{
  int idx = 0;
  Q_FOREACH(NodeItem *node, m_nodes) {
    node->setPos(m_oldPos[idx]);
    node->scene()->update();
    idx++;
  }
}

void MoveCommand::redo()
{
  int idx = 0;
  Q_FOREACH(NodeItem *node, m_nodes) {
    node->setPos(newPos[idx]);
    node->scene()->update();
    idx++;
  }
}

DeleteCommand::DeleteCommand(const QList< QGraphicsItem* > &items, GraphicsView *view,
			     QUndoCommand *parent)
  : QUndoCommand(parent)
{
  Q_FOREACH(QGraphicsItem *item, items) {
    if(item->type() == NodeItemType) {
      NodeItem *node = static_cast<NodeItem*>(item);
      m_nodes.append(node);
    }
    else if(item->type() == EdgeItemType) {
      EdgeItem *edge = static_cast<EdgeItem*>(item);
      m_edges.append(edge);
    }
    else if(item->type() == LoopItemType) {
      LoopItem *loop = static_cast<LoopItem*>(item);
      m_loops.append(loop);
    }
  }
  m_scene = view->scene();
  m_view = view;
}

void DeleteCommand::undo()
{
  Q_FOREACH(LoopItem *loopItem, m_loops) {
    if(loopItem->node()->scene() != NULL) {
      m_scene->addItem(loopItem);
      (*m_view->multipleLoopTable())[loopItem->node()->id()].append(loopItem);
    }
  }
  Q_FOREACH(NodeItem *nodeItem, m_nodes) {
    m_scene->addItem(nodeItem);
    Q_FOREACH(EdgeItem *edgeItem, nodeItem->edgeList()) {
      if(edgeItem->nodeU()->scene() != NULL && edgeItem->nodeV()->scene() != NULL) {
	m_scene->addItem(edgeItem);
      }
    }
    Q_FOREACH(LoopItem* loopItem, nodeItem->loopList()) {
      m_scene->addItem(loopItem);
      (*m_view->multipleLoopTable())[loopItem->node()->id()].append(loopItem);
    }
    m_scene->update();
  }
  Q_FOREACH(EdgeItem *edgeItem, m_edges) {
    if(edgeItem->nodeU()->scene() != NULL && edgeItem->nodeV()->scene() != NULL) {
      m_scene->addItem(edgeItem);
      edgeItem->nodeU()->addEdgeItem(edgeItem);
      edgeItem->nodeV()->addEdgeItem(edgeItem);

      pll nodePair = GraphicsView::nodeItemPair(edgeItem->nodeU(), edgeItem->nodeV());
      (*m_view->multipleEdgeTable())[nodePair].append(edgeItem);

      edgeItem->updateMultiTableIndex();
    }
  }
}

void DeleteCommand::redo()
{
  Q_FOREACH(EdgeItem *edgeItem, m_edges) {
    pll nodePair = GraphicsView::nodeItemPair(edgeItem->nodeU(), edgeItem->nodeV());
    (*m_view->multipleEdgeTable())[nodePair].removeAt(edgeItem->multiTableIndex());
    edgeItem->nodeU()->removeAllEdgeList(edgeItem);
    edgeItem->nodeV()->removeAllEdgeList(edgeItem);
    m_scene->removeItem(edgeItem);
    m_scene->update();
    Q_FOREACH(EdgeItem *edge, (*m_view->multipleEdgeTable())[nodePair]) {
      edge->updateMultiTableIndex();
    }
  }
  Q_FOREACH(LoopItem *loopItem, m_loops) {
    (*m_view->multipleLoopTable())[loopItem->node()->id()].removeAt(loopItem->multiTableIndex());
    m_scene->removeItem(loopItem);
    m_scene->update();
  }
  Q_FOREACH(NodeItem *nodeItem, m_nodes) {
    Q_FOREACH(EdgeItem *edgeItem, nodeItem->edgeList()) {
      if(edgeItem->scene() != NULL)
	m_scene->removeItem(edgeItem);
    }
    Q_FOREACH(LoopItem *loopItem, nodeItem->loopList()) {
      if(loopItem->scene() != NULL)
	m_scene->removeItem(loopItem);
    }
    m_scene->removeItem(nodeItem);
    m_scene->update();
  }
}

InfoCommand::InfoCommand(QList< QGraphicsItem* > items, PropertyDialog *dialog,
			 GraphicsView *view, QUndoCommand *parent)
  : QUndoCommand(parent)
{
  m_items = items;
  m_dialog = dialog;
  m_view = view;

  int length = items.count();
  oldName = QVector< QString >(length);
  oldValue = QVector< double >(length);
  oldFlow = QVector< double >(length);
  oldCap = QVector< double >(length);
  oldWidth = QVector< double >(length);
  oldSize = QVector< double >(length);
  oldColor = QVector< QColor >(length);
  oldFontSize = QVector< int >(length);
  oldCorner = QVector< int >(length);

  long long int idx = 0;
  Q_FOREACH(QGraphicsItem *item, m_items) {
    if(item->type() == NodeItemType) {
      NodeItem *nodeItem = static_cast<NodeItem*>(item);
      oldName[idx]  = nodeItem->name();
      oldValue[idx] = nodeItem->value();
      oldWidth[idx] = nodeItem->width();
      oldSize[idx]  = nodeItem->size();
      oldColor[idx] = nodeItem->color();
    }
    else if(item->type() == EdgeItemType) {
      EdgeItem *edgeItem = static_cast<EdgeItem*>(item);
      oldValue[idx]    = edgeItem->value();
      oldFlow[idx]     = edgeItem->flow();
      oldCap[idx]      = edgeItem->capacity();
      oldFontSize[idx] = edgeItem->fontSize();
      oldWidth[idx]    = edgeItem->width();
      oldColor[idx]    = edgeItem->color();
    }
    else if(item->type() == LoopItemType) {
      LoopItem *loopItem = static_cast<LoopItem*>(item);
      oldValue[idx]    = loopItem->value();
      oldWidth[idx]    = loopItem->width();
      oldColor[idx]    = loopItem->color();
      oldFontSize[idx] = loopItem->fontSize();
      oldCorner[idx]   = loopItem->corner();
    }
    idx++;
  }

  double value;
  if(dialog->valueLineEdit->text().size() == 0)
    value = std::numeric_limits< double >::quiet_NaN();
  else
    value = dialog->valueLineEdit->text().toDouble();

  double flow;
  if(dialog->flowLineEdit->text().size() == 0)
    flow = std::numeric_limits< double > ::quiet_NaN();
  else
    flow = dialog->flowLineEdit->text().toDouble();

  double capacity;
  if(dialog->capacityLineEdit->text().size() == 0)
    capacity = std::numeric_limits< double >::quiet_NaN();
  else
    capacity = dialog->capacityLineEdit->text().toDouble();

  newValue = QVector< double >(length, value);
  newFlow = QVector< double >(length, flow);
  newCap = QVector< double >(length, capacity);
  newName = QVector< QString >(length, dialog->nameLineEdit->text());
  newWidth = QVector< double >(length, dialog->widthLineEdit->text().toDouble());
  newSize = QVector< double >(length, dialog->sizeLineEdit->text().toDouble());
  newColor = QVector< QColor >(length, dialog->color);
  newFontSize = 
    QVector< int >(length, (dialog->fontSizeComboBox->currentIndex()+5)*2);
  newCorner = QVector< int >(length, dialog->cornerComboBox->currentIndex());
}

void InfoCommand::undo()
{
  long long int idx = 0;
  Q_FOREACH(QGraphicsItem *item, m_items) {
    if(item->type() == NodeItemType) {
      NodeItem *nodeItem = static_cast<NodeItem*>(item);
      nodeItem->setName(oldName[idx]);
      nodeItem->setValue(oldValue[idx]);
      nodeItem->setWidth(oldWidth[idx]);
      nodeItem->setSize(oldSize[idx]);
      nodeItem->setColor(oldColor[idx]);
    }
    else if(item->type() == EdgeItemType) {
      EdgeItem *edgeItem = static_cast<EdgeItem*>(item);
      edgeItem->setValue(oldValue[idx]);
      edgeItem->setFlow(oldFlow[idx]);
      edgeItem->setCapacity(oldCap[idx]);
      edgeItem->setFontSize(oldFontSize[idx]);
      edgeItem->setWidth(oldWidth[idx]);
      edgeItem->setColor(oldColor[idx]);
    }
    else if(item->type() == LoopItemType) {
      LoopItem *loopItem = static_cast<LoopItem*>(item);
      loopItem->setValue(oldValue[idx]);
      loopItem->setWidth(oldWidth[idx]);
      loopItem->setColor(oldColor[idx]);
      loopItem->setFontSize(oldFontSize[idx]);    
      loopItem->setCorner(oldCorner[idx]);
    }
    item->update();
    idx++;
  }
  m_view->scene()->update();
}

void InfoCommand::redo()
{
  long long int idx = 0;
  int diaType = m_dialog->dialogType();

  Q_FOREACH(QGraphicsItem *item, m_items) {
    if(item->type() == NodeItemType) {
      NodeItem *nodeItem = static_cast<NodeItem*>(item);
      if(diaType == 3 || diaType == 5 || diaType == 6)
	nodeItem->setName(oldName[idx]);
      else
	nodeItem->setName(newName[idx]);

      nodeItem->setValue(newValue[idx]);

      if(newWidth[idx] == 0)
	nodeItem->setWidth(oldWidth[idx]);
      else
	nodeItem->setWidth(newWidth[idx]);
      
      if(diaType == 3 || diaType == 5 || diaType == 6)
	nodeItem->setSize(oldSize[idx]);
      else
	nodeItem->setSize(newSize[idx]);

      nodeItem->setColor(newColor[idx]);
    }
    else if(item->type() == EdgeItemType) {
      EdgeItem *edgeItem = static_cast<EdgeItem*>(item);

      if(diaType == 1 || diaType == 4)
	edgeItem->setValue(newValue[idx]);

      if(diaType == 1) {
	edgeItem->setFlow(newFlow[idx]);
	edgeItem->setCapacity(newCap[idx]);
      }

      if(diaType == 3 || diaType == 6)
	edgeItem->setFontSize(oldFontSize[idx]);
      else
	edgeItem->setFontSize(newFontSize[idx]);

      if(newWidth[idx] == 0)
	edgeItem->setWidth(oldWidth[idx]);
      else
	edgeItem->setWidth(newWidth[idx]);

      edgeItem->setColor(newColor[idx]);
    }
    else if(item->type() == LoopItemType) {
      LoopItem *loopItem = static_cast<LoopItem*>(item);

      if(diaType == 2 || diaType == 4)
	loopItem->setValue(newValue[idx]);

      if(newWidth[idx] == 0)
	loopItem->setWidth(oldWidth[idx]);
      else
	loopItem->setWidth(newWidth[idx]);
      loopItem->setColor(newColor[idx]);

      if(diaType == 5 || diaType == 6)
	loopItem->setFontSize(oldFontSize[idx]);
      else
	loopItem->setFontSize(newFontSize[idx]);    

      loopItem->setCorner(newCorner[idx]);
    }
    item->update();
    idx++;
  }

  m_view->scene()->update();
}
