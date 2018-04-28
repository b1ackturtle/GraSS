#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"
#include "commands.h"

#include "graphicsview.h"

NodeItem::NodeItem(const QPointF &position, const long long int id)
  : QGraphicsEllipseItem()
{
  nodeID = id;
  m_name = QString ::number(nodeID);
  m_color = Qt::black;
  m_value = std::numeric_limits<double>::quiet_NaN();
  m_width = 3;
  m_size = 50;
  m_pos = position;

  setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsScenePositionChanges);

  setRect(m_pos.x()-halfsize(), m_pos.y()-halfsize(), size(), size());
}

NodeItem::NodeItem(const NodeItem &n) :QGraphicsEllipseItem()
{
  nodeID = n.id();
  m_name = n.name();
  m_color = n.color();
  m_value = n.value();
  m_width = n.width();
  m_size = n.size();
  m_pos = n.position();
  setPos(n.pos());

  setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsScenePositionChanges);

  setRect(m_pos.x()-halfsize(), m_pos.y()-halfsize(), size(), size());
}

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
		     QWidget *widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);

  QFont font = painter->font();
  font.setPointSize(rect().width()/2);
  
  setRect(m_pos.x()-halfsize(), m_pos.y()-halfsize(), size(), size());

  painter->setPen(QPen(QBrush(color()), width()));
  painter->setBrush(Qt::white);
  painter->drawEllipse(rect());

  painter->setFont(font);
  QFontMetrics fm(font);
  int fontWidth = fm.width(name()); int fontHeight = fm.height();
  QPointF center = boundingRect().center();
  painter->drawText(QRectF(center - QPointF(fontWidth/2, fontHeight/2),
			  QSizeF(fontWidth, fontHeight)), Qt::AlignCenter, name());

  if(isSelected()) {
    painter->setPen(QPen(option->palette.text(), 1.5, Qt::DashLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(boundingRect());
  }
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
  if(change == ItemPositionChange && scene()) {
    QPointF newPos = value.toPointF() + boundingRect().center();

    Q_FOREACH(EdgeItem *edgeItem, edgeList()) {
      if(edgeItem->scene() == NULL) continue;
      if(edgeItem->nodeU() == this)
	edgeItem->setU(newPos);
      else if(edgeItem->nodeV() == this)
	edgeItem->setV(newPos);
    }
    Q_FOREACH(LoopItem *loopItem, loopList()) {
      if(loopItem->scene() == NULL) continue;
      loopItem->setScreenPos(newPos);
    }
  }

  return QGraphicsEllipseItem::itemChange(change, value);
}

QDataStream &operator<<(QDataStream &out, const NodeItem &node)
{
  out << node.id() << node.pos() << node.name() << node.value() << node.color()
      << node.width() << node.size() << node.position();

  out << static_cast<qint32>(node.edgeList().size());
  for(int i = 0; i < node.edgeList().length(); i++)
    out << (qint64)node.edgeList().at(i);

  out << static_cast<qint32>(node.loopList().size());
  for(int i = 0; i < node.loopList().length(); i++)
    out << (qint64)node.loopList().at(i);


  return out;
}

QDataStream &operator>>(QDataStream &in, NodeItem &node)
{
  QPointF pos;
  QPointF position;
  QString name;
  double value, width, size;
  QColor color;
  qint32 edgeListLength, loopListLength;
  qint64 edgeAddr;
  qint64 loopAddr;
  QList<EdgeItem*> edgeList;
  QList<LoopItem*> loopList;

  in >> pos >> name >> value >> color >> width >> size >> position;
  node.setPos(pos);
  node.setName(name);
  node.setValue(value);
  node.setColor(color);
  node.setWidth(width);
  node.setSize(size);
  node.setPosition(position);
  node.setRect(node.position().x()-node.halfsize(),
	       node.position().y()-node.halfsize(),
	       node.size(), node.size());

  in >> edgeListLength;
  for(int i = 0; i < edgeListLength; i++) {
    in >> edgeAddr;
    node.addEdgeItem((EdgeItem*)edgeAddr);
  }

  in >> loopListLength;
  for(int i = 0; i < loopListLength; i++) {
    in >> loopAddr;
    node.addLoopItem((LoopItem*)loopAddr);
  }

  return in;
}
