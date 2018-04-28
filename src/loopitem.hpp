#ifndef LOOPITEM_HPP
#define LOOPITEM_HPP

#include "tools.hpp"
#include <QGraphicsEllipseItem>

class QColor;
class NodeItem;
class QDataStream;

class LoopItem : public QGraphicsEllipseItem
{
public:
  enum {Type = LoopItemType};

  explicit LoopItem(QPointF p, NodeItem *node, GraphType loopType);
  explicit LoopItem(const LoopItem &l, NodeItem *node);

  void setScreenPos(const QPointF &p);

  int type() const {return Type;}
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

  NodeItem* node() const {return m_node;}
  double value() const {return m_value;}
  QColor color() const {return m_color;}
  double width() const {return m_width;}
  double arrowSize() const {return m_arrowSize;}
  int fontSize() const {return m_fontSize;}
  int corner() const {return m_corner;}
  GraphType loopType() const {return m_loopType;}
  QPointF position() const {return m_pos;}
  long long int multiTableIndex() const {return m_multiTableIndex;}
  MultiLoopHash *multiTable() const {return m_multiTable;}

  void setNode(NodeItem *n) {m_node = n;}
  void setPosition(QPointF p) {m_pos = p;}
  void setValue(double value) {m_value = value;}
  void setColor(QColor color) {m_color = color;}
  void setWidth(double width) {m_width = width;}
  void setArrowSize(double size) {m_arrowSize = size;}
  void setFontSize(int fontSize) {m_fontSize = fontSize;}
  void setCorner(int corner) {m_corner = corner;}
  void setLoopType(GraphType type) {m_loopType = type;}
  void setMultiTable(MultiLoopHash *mt) {m_multiTable = mt;}

private:
  MultiLoopHash *m_multiTable;
  NodeItem *m_node;

  long long int m_multiTableIndex;

  double m_width;
  double m_arrowSize;
  double m_value;
  QColor m_color;
  int m_fontSize;
  int m_corner;
  GraphType m_loopType;

  QPointF m_pos;
};

QDataStream &operator<<(QDataStream &out, const LoopItem &loop);
QDataStream &operator>>(QDataStream &in, LoopItem &loop);

#endif
