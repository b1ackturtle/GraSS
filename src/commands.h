#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>

class GraphicsView;
class PropertyDialog;
class NodeItem;
class EdgeItem;
class LoopItem;

class AddNodeCommand : public QUndoCommand
{
public:
  AddNodeCommand(NodeItem *node, GraphicsView *view, QUndoCommand *parent = 0);
  ~AddNodeCommand();

  void undo();
  void redo();

private:
  NodeItem *m_node;
  QGraphicsScene *m_scene;
  GraphicsView *m_view;
};

class AddEdgeCommand : public QUndoCommand
{
public:
  AddEdgeCommand(EdgeItem *edge, GraphicsView *view, QUndoCommand *parent = 0);
  ~AddEdgeCommand();

  void undo();
  void redo();

private:
  EdgeItem *m_edge;
  QGraphicsScene *m_scene;
  GraphicsView *m_view;
};

class AddLoopCommand : public QUndoCommand
{
public:
  AddLoopCommand(LoopItem *loop, GraphicsView *view, QUndoCommand *parent = 0);
  ~AddLoopCommand();

  void undo();
  void redo();

private:
  LoopItem *m_loop;
  QGraphicsScene *m_scene;
  GraphicsView *m_view;
};

class MoveCommand : public QUndoCommand
{
public:
  MoveCommand(const QList< NodeItem* > &nodes, const QList< QPointF > &oldPos,
	      QUndoCommand *parent = 0);

  void undo();
  void redo();
  bool mergeWith(const QUndoCommand *command);
  
private:
  QList< NodeItem* > m_nodes;
  QList< QPointF > m_oldPos;
  QList< QPointF > newPos;
};

class DeleteCommand : public QUndoCommand
{
public:
  DeleteCommand(const QList< QGraphicsItem* > &items, GraphicsView *view,
		QUndoCommand *parent = 0);

  void undo();
  void redo();

private:
  QList< NodeItem* > m_nodes;
  QList< EdgeItem* > m_edges;
  QList< LoopItem* > m_loops;
  GraphicsView *m_view;
  QGraphicsScene *m_scene;
};

class InfoCommand : public QUndoCommand
{
public:
  InfoCommand(QList< QGraphicsItem* > items, PropertyDialog *dialog,
	      GraphicsView *view, QUndoCommand *parent = 0);

  void undo();
  void redo();

private:
  QList< QGraphicsItem* >m_items;
  PropertyDialog *m_dialog;
  GraphicsView *m_view;

  QVector< QString > newName, oldName;
  QVector< double > newValue, oldValue;
  QVector< double > newFlow, oldFlow;
  QVector< double > newCap, oldCap;
  QVector< double > newWidth, oldWidth;
  QVector< double > newSize, oldSize;
  QVector< QColor > newColor, oldColor;
  QVector< int > newFontSize, oldFontSize;
  QVector< int > newCorner, oldCorner;
};

#endif
