#ifndef EDGEITEM_HPP
#define EDGEITEM_HPP

#include "tools.hpp"
#include <QGraphicsLineItem>

class QColor;
class NodeItem;

class EdgeItem : public QGraphicsLineItem
{
public:
  enum {Type = EdgeItemType};

  explicit EdgeItem(QPointF u, QPointF v, NodeItem *nodeU, NodeItem *nodeV,
		    GraphType graphType);
  explicit EdgeItem(const EdgeItem &e, NodeItem *nodeU, NodeItem *nodeV);

  int type() const {return Type;}
  QRectF boundingRect() const;
  QPainterPath shape() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
  
  NodeItem* nodeU() const {return m_nodeU;}
  NodeItem* nodeV() const {return m_nodeV;}
  QPointF pointU() const {return m_u;}
  QPointF pointV() const {return m_v;}
  GraphType edgeType() const {return m_edgeType;}
  double value() const {return m_value;}
  double flow() const {return m_flow;}
  double capacity() const {return m_capacity;}
  int fontSize() const {return m_fontSize;}
  QColor color() const {return m_color;}
  double width() const {return m_width;}
  double arrowSize() const {return m_arrowSize;}
  long long int multiTableIndex() const {return m_multiTableIndex;}
  QLineF line() const {return m_line;}

  void setU(QPointF newU);
  void setV(QPointF newV);
  void setNodeU(NodeItem* newNodeU) {m_nodeU = newNodeU;}
  void setNodeV(NodeItem* newNodeV) {m_nodeV = newNodeV;}
  void setEdgeType(GraphType type) {m_edgeType = type;}
  void setValue(double value) {m_value = value;}
  void setFlow(double flow) {m_flow = flow;}
  void setCapacity(double capacity) {m_capacity = capacity;}
  void setFontSize(int fontSize) {m_fontSize = fontSize;}
  void setColor(QColor color) {m_color = color;}
  void setWidth(double width) {m_width = width;}
  void setArrowSize(double size) {m_arrowSize = size;}
  void setLine(qreal x1, qreal y1, qreal x2, qreal y2);
  void setMultiTable(MultiEdgeHash *mt) {m_multiTable = mt;}
  void updateMultiTableIndex();

private:
  void createSelectionPolygon();
  void createSelectionPath();
  QPainterPath path();

  const qreal selectionOffset;
  QPolygonF selectionPolygon;
  QPainterPath selectionPath;

  QLineF m_line;
  QPointF m_centerPoint;
  QPointF m_controlPoint;
  QPainterPath *m_path;
  long long int m_multiTableIndex;

  GraphType m_edgeType;
  MultiEdgeHash *m_multiTable;
  long long int multiTableSize;

  QPointF m_u;
  QPointF m_v;
  NodeItem *m_nodeU;
  NodeItem *m_nodeV;

  double m_width;
  double m_value;
  double m_flow;
  double m_capacity;
  int m_fontSize;
  QColor m_color;
  double m_arrowSize;
};

QDataStream &operator<<(QDataStream &out, const EdgeItem &edge);
QDataStream &operator>>(QDataStream &in, EdgeItem &edge);

#endif
