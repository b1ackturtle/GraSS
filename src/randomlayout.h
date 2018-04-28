#ifndef RANDOMLAYOUT_H
#define RANDOMLAYOUT_H

#include <QUndoCommand>
#include "graphdata.hpp"

class GraphicsView;
class NodeItem;

class RandomLayout : public QUndoCommand
{
public:
  RandomLayout(GraphicsView *view, QWidget *parent = 0);
  RandomLayout(GraphicsView *view, long long int n, std::vector< Edge > edges,
  	       QWidget *parent = 0);

  void run();
  void undo();
  void redo();

private:
  GraphicsView *m_view;
  QWidget *m_parent;
  std::vector< Edge > m_edges;
  long long int N;
  bool runFlag;

  QList<NodeItem*> nodeList;

  std::vector<QPointF> oldPos;
  std::vector<QPointF> newPos;
};

#endif
