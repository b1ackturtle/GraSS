#ifndef SPRINGLAYOUT_H
#define SPRINGLAYOUT_H

#include <QUndoCommand>

class GraphicsView;
class NodeItem;

class SpringLayout : public QUndoCommand
{
public:
  SpringLayout(GraphicsView *view, QWidget *parent = 0);

  void run(double coulomb, double bounce, Qt::CheckState);
  void undo();
  void redo();

private:
  GraphicsView *m_view;
  QWidget *m_parent;

  QList<NodeItem*> nodeList;

  std::vector<QPointF> oldPos;
  std::vector<QPointF> newPos;
};

#endif
