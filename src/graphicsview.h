#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include "tools.hpp"
#include "commands.h"

#include <unordered_map>
#include <map>

class QGraphicsScene;
class QMouseEvent;
class QUndoStack;
class PropertyDialog;
class NodeItem;
class EdgeItem;

class GraphicsView : public QGraphicsView
{
  Q_OBJECT

signals:
  void modifiedScene(bool flag);

public:
  enum ToolType {tNode, tEdge, tDelete, tMove, tInfo};
  
  explicit GraphicsView(QGraphicsScene *scene, QWidget *parent);
  explicit GraphicsView(const GraphicsView &g, QGraphicsScene *scn, QWidget *parent);

  GraphType graphType() const {return m_graphType;}
  MultiEdgeHash* multipleEdgeTable() const {return m_multipleEdgeTable;}
  MultiLoopHash* multipleLoopTable() const {return m_multipleLoopTable;}
  QUndoStack* undoStack() const {return m_undoStack;}

  ToolType toolFlag() const {return m_toolFlag;}
  void setToolFlag(ToolType flag) {m_toolFlag = flag;}
  void setMultipleEdgeTable(MultiEdgeHash *mt) {m_multipleEdgeTable = mt;}
  void setMultipleLoopTable(MultiLoopHash *mt) {m_multipleLoopTable = mt;}

  void multipleEdgeTableAppend(pll nodePair, EdgeItem *edge) {(*m_multipleEdgeTable)[nodePair].append(edge);}
  void multipleLoopTableAppend(long long int id, LoopItem *loop) {(*m_multipleLoopTable)[id].append(loop);}
  void multipleEdgeTableClear() {m_multipleEdgeTable->clear();}
  void multipleLoopTableClear() {m_multipleLoopTable->clear();}

  QPoint screenPos(QGraphicsItem *item);
  static pll nodeItemPair(NodeItem *n, NodeItem *m);

  long long int nodeId;

protected:
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);

public slots:
  void setGraphType(bool flag) {m_graphType = (flag) ? UNDIRECTED : DIRECTED;}

private:
  GraphType m_graphType;
  bool isItem;
  bool isPrevSelectedItem;
  QGraphicsItem *prevItem;
  ToolType m_toolFlag;
  QWidget *m_parent;

  MultiEdgeHash *m_multipleEdgeTable;
  MultiLoopHash *m_multipleLoopTable;

  PropertyDialog *dialog;

  QUndoStack *m_undoStack;
  QList< QPointF > m_oldPos;
};

#endif
