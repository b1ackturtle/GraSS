#ifndef NODEITEM_H
#define NODEITEM_H

#include "tools.hpp"
#include <QGraphicsEllipseItem>

class QColor;
class EdgeItem;
class LoopItem;
class QDataStream;

class NodeItem : public QGraphicsEllipseItem
{
public:
  enum {Type = NodeItemType};

  explicit NodeItem(const QPointF &position, long long int id);
  explicit NodeItem(const NodeItem &n);

  int type() const {return Type;}
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

  QString name() const {return m_name;}
  double value() const {return m_value;}
  QColor color() const {return m_color;}
  double width() const {return m_width;}
  double size() const {return m_size;}
  double halfsize() const {return m_size/2.0;}
  long long int id() const {return nodeID;}
  QPointF position() const {return m_pos;}
  QList<EdgeItem*> edgeList() const {return m_edgeList;}
  QList<LoopItem*> loopList() const {return m_loopList;}

  void setSize(const double size) {m_size = size;}
  void setValue(const double value) {m_value = value;}
  void setColor(const QColor color) {m_color = color;}
  void setWidth(const double width) {m_width = width;}
  void setName(const QString name) {m_name = name;}
  void setID(const long long int id) {nodeID = id;}
  void setPosition(const QPointF p) {m_pos = p;}
  void replaceEdgeList(const int i, EdgeItem *e) {m_edgeList.replace(i, e);}
  void replaceLoopList(const int i, LoopItem *l) {m_loopList.replace(i, l);}
  void removeAtEdgeList(int i) {m_edgeList.removeAt(i);}
  void removeAllEdgeList(EdgeItem* value) {m_edgeList.removeAll(value);}
  void removeAllLoopList(LoopItem* value) {m_loopList.removeAll(value);}
  void addEdgeItem(EdgeItem* edgeItem) {m_edgeList.append(edgeItem);}
  void addLoopItem(LoopItem* loopItem) {m_loopList.append(loopItem);}

protected:
  QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
  long long int nodeID;

  QString m_name;
  double m_value;
  QColor m_color;
  double m_width;
  double m_size;

  QList<EdgeItem*> m_edgeList;
  QList<LoopItem*> m_loopList;

  QPointF m_pos;
};

QDataStream &operator<<(QDataStream &out, const NodeItem &node);
QDataStream &operator>>(QDataStream &in, NodeItem &node);

#endif
