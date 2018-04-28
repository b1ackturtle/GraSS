#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "loopitem.hpp"
#include "nodeitem.hpp"

#include <iostream>

LoopItem::LoopItem(QPointF p, NodeItem *node, GraphType loopType)
{
  m_pos = p;
  m_node = node;
  m_value = std::numeric_limits<double>::quiet_NaN();
  m_width = 1.0;
  m_color = Qt::black;
  m_fontSize = 18;
  m_arrowSize = 15;
  m_corner = 0;
  m_loopType = loopType;

  setFlags(QGraphicsItem::ItemIsSelectable);
}

LoopItem::LoopItem(const LoopItem &l, NodeItem *node)
{
  m_pos = l.position();
  m_node = node;
  m_value = l.value();
  m_width = l.width();
  m_color = l.color();
  m_fontSize = l.fontSize();
  m_arrowSize = l.arrowSize();
  m_corner = l.corner();
  m_loopType = l.loopType();

  setFlags(QGraphicsItem::ItemIsSelectable);
}

void LoopItem::setScreenPos(const QPointF &p)
{
  m_pos = p;

  setRect(m_pos.x(), m_pos.y(), m_node->size(), m_node->size());
  scene()->update();
}

void LoopItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
		     QWidget *widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);

  m_multiTableIndex = m_multiTable->at(m_node->id()).indexOf(this);

  if(m_multiTableIndex != 0)
    m_corner = m_multiTable->at(m_node->id()).at(0)->corner();

  setZValue(-m_multiTableIndex-1);

  int cornerX[] = {+1, -1, +1, -1};
  int cornerY[] = {+1, +1, -1, -1};
  int cornerW[] = {+1, -1, +1, -1};

  double r = m_node->size() / 2.0 + 0.8 * m_multiTableIndex * m_node->size() / 2.0;

  double x1 = m_pos.x() + cornerX[m_corner] * m_node->size() / 2.0; double y1 = m_pos.y();
  double x2 = m_pos.x(); double y2 = m_pos.y() + cornerY[m_corner] * m_node->size() / 2.0;

  double centX = (x1 + x2) / 2.0; double centY = (y1 + y2) / 2.0;
  double lengX = (x1 - x2); double lengY = (y1 - y2);
  double a = centX + cornerW[m_corner] * sqrt((r*r-(lengX/2)*(lengX/2)-(lengY/2)*(lengY/2))/(1+(lengX/lengY)*(lengX/lengY)));
  double b = centY - cornerW[m_corner] * lengX/lengY * sqrt((r*r-(lengX/2)*(lengX/2)-(lengY/2)*(lengY/2))/(1+(lengX/lengY)*(lengX/lengY)));
  setRect(a - r, b - r, 2 * r, 2 * r);

  painter->setPen(QPen(QBrush(color()), width()));
  painter->drawArc(rect(), startAngle(), spanAngle());

  if(!std::isnan(value())) {
    QFont font = painter->font();
    font.setPointSize(fontSize());
    painter->setFont(font);
    painter->drawText(QPointF(a+cornerX[m_corner]*r/sqrt(2),
			      b+cornerY[m_corner]*r/sqrt(2)),
		      QString::number(value()));
  }

  if(m_loopType == DIRECTED) {
    painter->setPen(QPen(color()));
    painter->setBrush(color());

    QPointF p = QPointF(a, b - cornerY[m_corner] * r);
    QPointF arrowP1 = p + QPointF(sin(cornerX[m_corner]*M_PI/3) * arrowSize(),
     				  cos(cornerX[m_corner]*M_PI/3) * arrowSize());
    QPointF arrowP2 = p + QPointF(sin(cornerX[m_corner]*(M_PI-M_PI/3))*arrowSize(),
				  cos(cornerX[m_corner]*(M_PI-M_PI/3))*arrowSize());

    QPolygonF arrowHead;
    arrowHead << p << arrowP1 << arrowP2;
    painter->drawPolygon(arrowHead);
  }

  if(isSelected()) {
    painter->setPen(QPen(option->palette.text(), 1.5, Qt::DashLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(boundingRect());
  }
}

QDataStream &operator<<(QDataStream &out, const LoopItem &loop)
{
  out << (qint64)&loop << (qint64)loop.node()->id() << loop.value()
      << loop.fontSize() << loop.color() << loop.width() << loop.position();
  return out;
}

QDataStream &operator>>(QDataStream &in, LoopItem &loop)
{
  qint64 id;
  QPointF position;
  double value, width;
  int fontSize;
  QColor color;

  in >> id >> value >> fontSize >> color >> width >> position;
  loop.node()->setID(id);
  loop.setValue(value);
  loop.setFontSize(fontSize);
  loop.setColor(color);
  loop.setWidth(width);
  loop.setPosition(position);

  return in;
}
