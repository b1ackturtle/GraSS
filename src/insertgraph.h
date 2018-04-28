#ifndef INSERTGRAPH_H
#define INSERTGRAPH_H

#include <QUndoCommand>

class GraphicsView;
class NodeItem;
class EdgeItem;

class InsertGraph : public QUndoCommand
{
public:
  InsertGraph(GraphicsView *view, QWidget *parent = 0);

  void undo();
  void redo();

protected:
  GraphicsView *m_view;
  QWidget *m_parent;

  QVector< NodeItem* > nodes;
  QVector< EdgeItem* > edges;
};

#endif
