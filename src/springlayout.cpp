#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "springlayout.h"
#include "graphicsview.h"
#include "mainwindow.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"
#include "tools.hpp"

#include <QTimer>

#include <vector>
#include <map>

SpringLayout::SpringLayout(GraphicsView *view, QWidget *parent)
  : QUndoCommand(0)
{
  m_view = view;
  m_parent = parent;

  oldPos = std::vector<QPointF>(view->nodeId);
  newPos = std::vector<QPointF>(view->nodeId);
}

void SpringLayout::run(double coulomb, double bounce, Qt::CheckState animation)
{
  QList<EdgeItem*> edgeList;
  QList<LoopItem*> loopList;
  std::vector< std::pair<double, double> > velocity(m_view->nodeId,
						    std::make_pair(0.0, 0.0));

  Q_FOREACH(QGraphicsItem *item, m_view->scene()->items()) {
    if(item->type() == NodeItemType) {
      NodeItem *node = static_cast<NodeItem*>(item);
      nodeList.push_back(node);
      oldPos[node->id()] = node->pos();
    }
    else if(item->type() == EdgeItemType)
      edgeList.push_back(static_cast<EdgeItem*>(item));
    else if(item->type() == LoopItemType)
      loopList.push_back(static_cast<LoopItem*>(item));
  }

  double fx, fy;
  double attenuation = 0.8;
  for(int itr = 0; itr < 100000; itr++) {
    double energy = 0.0;
    Q_FOREACH(NodeItem *node1, nodeList) {
      if(node1->edgeList().size() == 0) continue;
      fx = 0.0; fy = 0.0;
      Q_FOREACH(NodeItem *node2, nodeList) {
	if(node1 == node2) continue;
	if(node2->edgeList().size() == 0) continue;
	QPointF pos1 = node1->position() + node1->pos();
	QPointF pos2 = node2->position() + node2->pos();
	double distX = pos1.x() - pos2.x(); 
	double distY = pos1.y() - pos2.y();
	double length = distX * distX + distY * distY;

	fx += coulomb * distX / length;
	fy += coulomb * distY / length;
      }
      
      Q_FOREACH(EdgeItem *edge, node1->edgeList()) {
	NodeItem *node2;
	if(edge->nodeU() == node1) node2 = edge->nodeV();
	else node2 = edge->nodeU();

	QPointF pos1 = node1->position() + node1->pos();
	QPointF pos2 = node2->position() + node2->pos();
	QPointF dist = pos2 - pos1;
	fx += bounce * dist.x();
	fy += bounce * dist.y();
      }

      velocity[node1->id()].first =
	(velocity[node1->id()].first + fx) * attenuation;
      velocity[node1->id()].second =
	(velocity[node1->id()].second + fy) * attenuation;

      double newX = node1->pos().x() + velocity[node1->id()].first;
      double newY = node1->pos().y() + velocity[node1->id()].second;
      node1->setPos(QPointF(newX, newY));
      newPos[node1->id()] = node1->pos();

      m_view->scene()->update();

      energy += sqrt(fx * fx + fy * fy);
    }
    if(energy <= 0.1) break;

    if(animation == Qt::Checked)
      QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
  }

  if(animation == Qt::Checked)
    QApplication::processEvents();
}

void SpringLayout::undo()
{
  Q_FOREACH(NodeItem *node, nodeList) {
    node->setPos(oldPos[node->id()]);
  }
}

void SpringLayout::redo()
{
  Q_FOREACH(NodeItem *node, nodeList) {
    node->setPos(newPos[node->id()]);
  }
}
