#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "edgeitem.hpp"
#include "nodeitem.hpp"

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

EdgeItem::EdgeItem(QPointF u, QPointF v, NodeItem *nodeU, NodeItem *nodeV,
		   GraphType edgeType)
  : QGraphicsLineItem(), selectionOffset(10)
{
  m_width = 1.0;
  m_value = std::numeric_limits<double>::quiet_NaN();
  m_flow = std::numeric_limits<double>::quiet_NaN();
  m_capacity = std::numeric_limits<double>::quiet_NaN();
  m_fontSize = 18;
  m_color = Qt::black;
  m_arrowSize = 15;
  m_u = u;
  m_v = v;
  m_nodeU = nodeU;
  m_nodeV = nodeV;
  m_edgeType = edgeType;
  m_path = NULL;

  setFlags(QGraphicsItem::ItemIsSelectable);
  setZValue(-2);
  setLine(u.x(), u.y(), v.x(), v.y());

  createSelectionPolygon();
}

EdgeItem::EdgeItem(const EdgeItem &e, NodeItem *nodeU, NodeItem *nodeV)
  : QGraphicsLineItem(), selectionOffset(10)
{
  m_width = e.width();
  m_value = e.value();
  m_flow = e.flow();
  m_capacity = e.capacity();
  m_fontSize = e.fontSize();
  m_color = e.color();
  m_arrowSize = e.arrowSize();
  m_u = e.pointU();
  m_v = e.pointV();
  m_nodeU = nodeU;
  m_nodeV = nodeV;
  m_edgeType = e.edgeType();
  m_path = NULL;

  setFlags(QGraphicsItem::ItemIsSelectable);
  setZValue(-2);
  setLine(m_u.x(), m_u.y(), m_v.x(), m_v.y());

  createSelectionPolygon();
}

void EdgeItem::createSelectionPolygon()
{
  QPolygonF nPolygon;
  qreal radAngle = line().angle()* M_PI / 180;
  qreal dx = selectionOffset * sin(radAngle);
  qreal dy = selectionOffset * cos(radAngle);
  QPointF offset1 = QPointF(dx, dy);
  QPointF offset2 = QPointF(-dx, -dy);
  nPolygon << line().p1() + offset1
	   << line().p1() + offset2
	   << line().p2() + offset2
	   << line().p2() + offset1;
  selectionPolygon = nPolygon;
}

void EdgeItem::createSelectionPath()
{
  QPainterPath nPath, path1, path2, path3, path4;
  qreal radAngle = line().angle() * M_PI / 180;
  qreal dx = selectionOffset * sin(radAngle);
  qreal dy = selectionOffset * cos(radAngle);
  QPointF offset1 = QPointF(dx, dy);
  QPointF offset2 = QPointF(-dx, -dy);

  path1.moveTo(line().p1() + offset1);
  path1.quadTo(line().p1(), line().p1() + offset2);
  path2 = path();
  path2.translate(offset1);
  path3.moveTo(line().p2() + offset1);
  path3.quadTo(line().p2(), line().p2() + offset2);
  path4 = path();
  path4.translate(offset2);

  nPath.addPath(path1);
  nPath.addPath(path2);
  nPath.addPath(path3);
  nPath.addPath(path4);

  selectionPath = nPath;
}

void EdgeItem::setU(QPointF newU)
{
  m_u = newU;
  setLine(m_u.x(), m_u.y(), m_v.x(), m_v.y());
}

void EdgeItem::setV(QPointF newV)
{
  m_v = newV;
  setLine(m_u.x(), m_u.y(), m_v.x(), m_v.y());
}

void EdgeItem::setLine(qreal x1, qreal y1, qreal x2, qreal y2)
{
  if(m_line == QLineF(x1, y1, x2, y2))
    return;
  prepareGeometryChange();
  m_line = QLineF(x1, y1, x2, y2);
  update();
}

void EdgeItem::updateMultiTableIndex()
{
  pll nodePair;
  if(nodeU()->id() > nodeV()->id())
    nodePair = pll(nodeU()->id(), nodeV()->id());
  else
    nodePair = pll(nodeV()->id(), nodeU()->id());
  m_multiTableIndex = m_multiTable->at(nodePair).indexOf(this);
}

QRectF EdgeItem::boundingRect() const
{
  return shape().controlPointRect();
}

QPainterPath EdgeItem::shape() const
{
  QPainterPath ret;
  if(m_multiTableIndex == 0 && multiTableSize % 2) ret.addPolygon(selectionPolygon);
  else ret.addPath(selectionPath);
  return ret;
}

void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, 
		     QWidget *widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);

  if(m_multiTableIndex == 0) setZValue(-1);

  QGraphicsLineItem::setLine(m_u.x(), m_u.y(), m_v.x(), m_v.y());

  createSelectionPath();
  createSelectionPolygon();

  setArrowSize(15 * width());

  painter->setPen(QPen(QBrush(color()), width()));
  painter->drawPath(path());

  QFont font = painter->font();
  font.setPointSize(fontSize());
  painter->setFont(font);
  QString edgeText;

  if(!std::isnan(value())) {
    if(std::isnan(capacity()))
      edgeText = QString::number(value());
    else if(std::isnan(flow()))
      edgeText = QString::number(value()) + ";" + QString::number(capacity());
    else
      edgeText = 
	QString::number(value()) + ";" +
	QString::number(flow()) + "/" + QString::number(capacity());
  }
  else if(!std::isnan(capacity())) {
    if(std::isnan(flow()))
      edgeText = QString::number(capacity());
    else
      edgeText = QString::number(flow()) + "/" + QString::number(capacity());
  }
  QFontMetrics fm(font);
  int fontWidth = fm.width(edgeText); int fontHeight = fm.height();
  painter->drawText(QRectF(m_centerPoint - QPointF(fontWidth/2, fontHeight/2),
			   QSizeF(fontWidth, fontHeight)), Qt::AlignCenter, edgeText);

  if(m_edgeType == DIRECTED) {
    double px, py, t;
    double cx = line().p1().x();
    double cy = line().p1().y();
    for(t = 1.0; t >= 0; t-=0.001) {
      px = (1-t)*(1-t)*line().p1().x()+2*(1-t)*t*m_controlPoint.x()
	+t*t*line().p2().x();
      py = (1-t)*(1-t)*line().p1().y()+2*(1-t)*t*m_controlPoint.y()
	+t*t*line().p2().y();
      double R =  sqrt((px-cx)*(px-cx)+(py-cy)*(py-cy));
      if(R > nodeU()->halfsize()-0.5 && R < nodeU()->halfsize()+0.5) break;
    }
    painter->setPen(QPen(color()));
    painter->setBrush(color());

    double dxdt =
      -2*(1-t)*line().p1().x()+2*(1-2*t)*m_controlPoint.x()+2*t*line().p2().x();
    double dydt =
      -2*(1-t)*line().p1().y()+2*(1-2*t)*m_controlPoint.y()+2*t*line().p2().y();
    double dydx = dydt/dxdt;

    double angle = (dxdt < 0) ? M_PI - fabs(atan(dydx)) : fabs(atan(dydx));

    if(dydt >= 0) angle = 2*M_PI - angle;

    QPointF arrowP1 = QPointF(px, py) + QPointF(sin(angle + M_PI / 3) * arrowSize(),
					    cos(angle + M_PI / 3) * arrowSize());
    QPointF arrowP2 = QPointF(px, py) + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize(), cos(angle + M_PI - M_PI / 3) * arrowSize());

    QPolygonF arrowHead;
    arrowHead << QPointF(px, py) << arrowP1 << arrowP2;
    painter->drawPolygon(arrowHead);
  }

  if(isSelected()) {
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(Qt::black, 1.5, Qt::DashLine));
    painter->drawPath(selectionPath);
  }
}

QPainterPath EdgeItem::path()
{
  QPointF offset;
  double hDiv, h = 0;
  double x, y;
  double a, b, c, d, u, v;
  pll nodePair;
  long long int heightOffset = 200;

  a = line().p1().x(); b = line().p1().y();
  c = line().p2().x(); d = line().p2().y();
  u = (a + c) / 2;
  v = (b + d) / 2;

  if(nodeU()->id() > nodeV()->id()) {
    nodePair = pll(nodeU()->id(), nodeV()->id());
    heightOffset *= -1;
  }
  else nodePair = pll(nodeV()->id(), nodeU()->id());

  long long int mult = m_multiTable->at(nodePair).size();

  if(m_path != NULL) delete m_path;
  m_path = new QPainterPath();
  multiTableSize = mult;
  long long int num = m_multiTable->at(nodePair).indexOf(this);

  if(mult % 2 && mult != 1) hDiv = heightOffset / (mult - 1);
  else hDiv = heightOffset / mult;

  if(mult % 2) {
    if(num == 0) h = 0;
    else if(num % 2) h = hDiv * ((num + 1) / 2);
    else if(!(num % 2)) h = -hDiv * (num / 2);
  }
  else if(!(mult % 2)) {
    if(num % 2) h = hDiv * ((num + 1) / 2);
    else if(!(num % 2)) h = -hDiv * ((num / 2) + 1);
  }

  x = u + (h * (b - d)) / (sqrt((a - c) * (a - c) + (b - d) * (b - d)));
  y = v - (h * (a - c)) / (sqrt((a - c) * (a - c) + (b - d) * (b - d)));

  offset = QPointF(x, y);

  double centerX = 0.5*0.5*line().p2().x()+2*0.5*0.5*offset.x()+0.5*0.5*line().p1().x();
  double centerY = 0.5*0.5*line().p2().y()+2*0.5*0.5*offset.y()+0.5*0.5*line().p1().y();

  m_centerPoint = QPointF(centerX, centerY);
  m_controlPoint = offset;
  m_path->moveTo(line().p1());
  m_path->quadTo(offset, line().p2());
  m_multiTableIndex = num;
  return *m_path;
}

QDataStream &operator<<(QDataStream &out, const EdgeItem &edge)
{
  out << (qint64)&edge << (qint64)edge.nodeU()->id() << (qint64)edge.nodeV()->id()
      << edge.pointU() << edge.pointV() << (edge.edgeType() == UNDIRECTED)
      << edge.value() << edge.flow() << edge.capacity() << edge.fontSize() << edge.color()
      << edge.width() << edge.arrowSize();
  return out;
}

QDataStream &operator>>(QDataStream &in, EdgeItem &edge)
{
  qint64 idU, idV;
  QPointF pointU, pointV;
  bool edgeType;
  double value, flow, capacity, width, arrowSize;
  int fontSize;
  QColor color;
  
  in >> idU >> idV >> pointU >> pointV >> edgeType >> value >> flow >> capacity 
     >> fontSize >> color >> width >> arrowSize;
  edge.nodeU()->setID(idU); edge.nodeV()->setID(idV);
  edge.setU(pointU); edge.setV(pointV);
  edge.setEdgeType(edgeType ? UNDIRECTED : DIRECTED);
  edge.setValue(value);
  edge.setFlow(flow);
  edge.setCapacity(capacity);
  edge.setWidth(width);
  edge.setArrowSize(arrowSize);
  edge.setFontSize(fontSize);
  edge.setColor(color);

  return in;
}
