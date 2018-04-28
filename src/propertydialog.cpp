#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "propertydialog.h"
#include "tools.hpp"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"

#include <QColorDialog>

PropertyDialog::PropertyDialog(QList< QGraphicsItem* > items, QWidget *parent)
  : QDialog(parent)
{
  m_items = items;

  itemType = std::vector< bool >(3, false);
  Q_FOREACH(QGraphicsItem *item, items) {
    if(item->type() == NodeItemType) {
      itemType[0] = true;
    }
    else if(item->type() == EdgeItemType) {
      itemType[1] = true;
    }
    else if(item->type() == LoopItemType) {
      itemType[2] = true;
    }
  }
  
  createWidgets();
  createLayouts();
  createConnections();

  setVisibleAndValue();

  setFixedHeight(sizeHint().height());

  if(dialogType() == 0)
    setWindowTitle(tr("Node Property"));
  else if(dialogType() == 1 || dialogType() == 2 || dialogType() == 4)
    setWindowTitle(tr("Edge Property"));
  else
    setWindowTitle(tr("Item Property"));
}

void PropertyDialog::setVisibleAndValue()
{
  NodeItem *nodeItem;
  EdgeItem *edgeItem;
  LoopItem *loopItem;
  bool diffName = false;
  bool diffValue = false;
  bool diffFlow = false;
  bool diffCap = false;
  bool diffColor = false;
  bool diffWidth = false;
  bool diffSize = false;
  bool diffFontSize = false;
  bool diffCorner = false;

  switch(dialogType())
    {
    case 0: { //Only Node
      nodeItem = static_cast<NodeItem*>(m_items.front());

      Q_FOREACH(QGraphicsItem *item, m_items) {
	NodeItem *node = static_cast<NodeItem*>(item);
	if(node->name() != nodeItem->name()) diffName = true;
	if(node->value() != nodeItem->value()) diffValue = true;
	if(node->color() != nodeItem->color()) diffColor = true;
	if(node->width() != nodeItem->width()) diffWidth = true;
	if(node->size() != nodeItem->size()) diffSize = true;
      }

      if(m_items.count() == 1)
	idLineEdit->setText(QString::number(nodeItem->id()));
      idLineEdit->setDisabled(true);

      cornerLabel->setVisible(false);
      cornerComboBox->setVisible(false);

      fontSizeLabel->setVisible(false);
      fontSizeComboBox->setVisible(false);

      valueLabel->setVisible(false);
      valueLineEdit->setVisible(false);

      flowLabel->setVisible(false);
      flowLineEdit->setVisible(false);
      capacityLabel->setVisible(false);
      capacityLineEdit->setVisible(false);

      if(!diffName)
	nameLineEdit->setText(nodeItem->name());
      if(!diffColor) {
	color = nodeItem->color();
	if(nodeItem->color() == Qt::black)
	  colorComboBox->setCurrentIndex(0);
	else if(nodeItem->color() == Qt::red)
	  colorComboBox->setCurrentIndex(1);
	else if(nodeItem->color() == Qt::green)
	  colorComboBox->setCurrentIndex(2);
	else if(nodeItem->color() == Qt::blue)
	  colorComboBox->setCurrentIndex(3);
	else
	  colorComboBox->setCurrentIndex(4);
      }
      if(!diffWidth)
	widthLineEdit->setText(QString::number(nodeItem->width()));
      if(!diffSize)
	sizeLineEdit->setText(QString::number(nodeItem->size()));
      
      break;
    }
    case 1: { //Only Edge
      edgeItem = static_cast<EdgeItem*>(m_items.front());

      Q_FOREACH(QGraphicsItem *item, m_items) {
	EdgeItem *edge = static_cast<EdgeItem*>(item);
	if(edge->value() != edgeItem->value()) diffValue = true;
	if(edge->flow() != edgeItem->flow()) diffFlow = true;
	if(edge->capacity() != edgeItem->capacity()) diffCap = true;
	if(edge->fontSize() != edgeItem->fontSize()) diffFontSize = true;
	if(edge->color() != edgeItem->color()) diffColor = true;
	if(edge->width() != edgeItem->width()) diffWidth = true;
      }

      idLabel->setVisible(false);
      idLineEdit->setVisible(false);

      nameLabel->setVisible(false);
      nameLineEdit->setVisible(false);

      sizeLabel->setVisible(false);
      sizeLineEdit->setVisible(false);

      cornerLabel->setVisible(false);
      cornerComboBox->setVisible(false);
    
      if(!diffValue && !isnan(edgeItem->value()))
	valueLineEdit->setText(QString::number(edgeItem->value()));
      if(!diffFlow && !isnan(edgeItem->flow()))
	flowLineEdit->setText(QString::number(edgeItem->flow()));
      if(!diffCap && !isnan(edgeItem->capacity()))
	capacityLineEdit->setText(QString::number(edgeItem->capacity()));
      if(!diffFontSize)
	fontSizeComboBox->setCurrentIndex(edgeItem->fontSize()/2-5);
      if(!diffColor) {
	color = edgeItem->color();
	if(edgeItem->color() == Qt::black)
	  colorComboBox->setCurrentIndex(0);
	else if(edgeItem->color() == Qt::red)
	  colorComboBox->setCurrentIndex(1);
	else if(edgeItem->color() == Qt::green)
	  colorComboBox->setCurrentIndex(2);
	else if(edgeItem->color() == Qt::blue)
	  colorComboBox->setCurrentIndex(3);
	else
	  colorComboBox->setCurrentIndex(4);
      }
      if(!diffWidth)
	widthLineEdit->setText(QString::number(edgeItem->width()));
      
      break;
    }
    case 2: { //Only Loop
      loopItem = static_cast<LoopItem*>(m_items.front());

      Q_FOREACH(QGraphicsItem *item, m_items) {
	LoopItem *loop = static_cast<LoopItem*>(item);
	if(loop->value() != loopItem->value()) diffValue = true;
	if(loop->fontSize() != loopItem->fontSize()) diffFontSize = true;
	if(loop->color() != loopItem->color()) diffColor = true;
	if(loop->width() != loopItem->width()) diffWidth = true;
	if(loop->corner() != loopItem->corner()) diffCorner = true;
      }

      idLabel->setVisible(false);
      idLineEdit->setVisible(false);

      nameLabel->setVisible(false);
      nameLineEdit->setVisible(false);

      sizeLabel->setVisible(false);
      sizeLineEdit->setVisible(false);

      flowLabel->setVisible(false);
      flowLineEdit->setVisible(false);
      capacityLabel->setVisible(false);
      capacityLineEdit->setVisible(false);

      if(!diffValue && !isnan(loopItem->value()))
	valueLineEdit->setText(QString::number(loopItem->value()));
      if(!diffFontSize)
	fontSizeComboBox->setCurrentIndex(loopItem->fontSize()/2-5);
      if(!diffColor) {
	color = loopItem->color();
	if(loopItem->color() == Qt::black)
	  colorComboBox->setCurrentIndex(0);
	else if(loopItem->color() == Qt::red)
	  colorComboBox->setCurrentIndex(1);
	else if(loopItem->color() == Qt::green)
	  colorComboBox->setCurrentIndex(2);
	else if(loopItem->color() == Qt::blue)
	  colorComboBox->setCurrentIndex(3);
	else
	  colorComboBox->setCurrentIndex(4);
      }
      if(!diffWidth)
	widthLineEdit->setText(QString::number(loopItem->width()));
      if(!diffCorner)
	cornerComboBox->setCurrentIndex(loopItem->corner());

      break;
    }
    case 4: { //Edge and Loop
      if(m_items.front()->type() == EdgeItemType) {
	edgeItem = static_cast<EdgeItem*>(m_items.front());

	Q_FOREACH(QGraphicsItem *item, m_items) {
	  if(item->type() == EdgeItemType) {
	    EdgeItem *edge = static_cast<EdgeItem*>(item);
	    if(edge->value() != edgeItem->value()) diffValue = true;
	    if(edge->fontSize() != edgeItem->fontSize()) diffFontSize = true;
	    if(edge->color() != edgeItem->color()) diffColor = true;
	    if(edge->width() != edgeItem->width()) diffWidth = true;
	  }
	  else if(item->type() == LoopItemType) {
	    LoopItem *loop = static_cast<LoopItem*>(item);
	    if(loop->value() != edgeItem->value()) diffValue = true;
	    if(loop->fontSize() != edgeItem->fontSize()) diffFontSize = true;
	    if(loop->color() != edgeItem->color()) diffColor = true;
	    if(loop->width() != edgeItem->width()) diffWidth = true;
	  }
	}

	if(!diffValue && !isnan(edgeItem->value()))
	  valueLineEdit->setText(QString::number(edgeItem->value()));
	if(!diffFontSize)
	  fontSizeComboBox->setCurrentIndex(edgeItem->fontSize()/2-5);
	if(!diffColor) {
	  color = edgeItem->color();
	  if(edgeItem->color() == Qt::black)
	    colorComboBox->setCurrentIndex(0);
	  else if(edgeItem->color() == Qt::red)
	    colorComboBox->setCurrentIndex(1);
	  else if(edgeItem->color() == Qt::green)
	    colorComboBox->setCurrentIndex(2);
	  else if(edgeItem->color() == Qt::blue)
	    colorComboBox->setCurrentIndex(3);
	  else
	    colorComboBox->setCurrentIndex(4);
	}
	if(!diffWidth)
	  widthLineEdit->setText(QString::number(edgeItem->width()));
      }
      else if(m_items.front()->type() == LoopItemType) {
	loopItem = static_cast<LoopItem*>(m_items.front());

	Q_FOREACH(QGraphicsItem *item, m_items) {
	  if(item->type() == EdgeItemType) {
	    EdgeItem *edge = static_cast<EdgeItem*>(item);
	    if(edge->value() != loopItem->value()) diffValue = true;
	    if(edge->fontSize() != loopItem->fontSize()) diffFontSize = true;
	    if(edge->color() != loopItem->color()) diffColor = true;
	    if(edge->width() != loopItem->width()) diffWidth = true;
	  }
	  else if(item->type() == LoopItemType) {
	    LoopItem *loop = static_cast<LoopItem*>(item);
	    if(loop->value() != loopItem->value()) diffValue = true;
	    if(loop->fontSize() != loopItem->fontSize()) diffFontSize = true;
	    if(loop->color() != loopItem->color()) diffColor = true;
	    if(loop->width() != loopItem->width()) diffWidth = true;
	  }
	}

	if(!diffValue && !isnan(loopItem->value()))
	  valueLineEdit->setText(QString::number(loopItem->value()));
	if(!diffFontSize)
	  fontSizeComboBox->setCurrentIndex(loopItem->fontSize()/2-5);
	if(!diffColor) {
	  color = loopItem->color();
	  if(loopItem->color() == Qt::black)
	    colorComboBox->setCurrentIndex(0);
	  else if(loopItem->color() == Qt::red)
	    colorComboBox->setCurrentIndex(1);
	  else if(loopItem->color() == Qt::green)
	    colorComboBox->setCurrentIndex(2);
	  else if(loopItem->color() == Qt::blue)
	    colorComboBox->setCurrentIndex(3);
	  else
	    colorComboBox->setCurrentIndex(4);
	}
	if(!diffWidth)
	  widthLineEdit->setText(QString::number(loopItem->width()));
      }

      idLabel->setVisible(false);
      idLineEdit->setVisible(false);

      nameLabel->setVisible(false);
      nameLineEdit->setVisible(false);

      sizeLabel->setVisible(false);
      sizeLineEdit->setVisible(false);

      cornerLabel->setVisible(false);
      cornerComboBox->setVisible(false);

      flowLabel->setVisible(false);
      flowLineEdit->setVisible(false);
      capacityLabel->setVisible(false);
      capacityLineEdit->setVisible(false);

      break;
    }
    case 3: //Node and Edge
    case 5: //Node and Loop
    case 6: //All Item(Node, Edge and Loop)
      {
      idLabel->setVisible(false);
      idLineEdit->setVisible(false);

      valueLabel->setVisible(false);
      valueLineEdit->setVisible(false);

      flowLabel->setVisible(false);
      flowLineEdit->setVisible(false);
      capacityLabel->setVisible(false);
      capacityLineEdit->setVisible(false);

      nameLabel->setVisible(false);
      nameLineEdit->setVisible(false);

      sizeLabel->setVisible(false);
      sizeLineEdit->setVisible(false);

      fontSizeLabel->setVisible(false);
      fontSizeComboBox->setVisible(false);

      cornerLabel->setVisible(false);
      cornerComboBox->setVisible(false);

      break;
    }
    }
}

int PropertyDialog::dialogType()
{
  if(itemType[0] && !itemType[1] && !itemType[2]) return 0;
  else if(!itemType[0] && itemType[1] && !itemType[2]) return 1;
  else if(!itemType[0] && !itemType[1] && itemType[2]) return 2;
  else if(itemType[0] && itemType[1] && !itemType[2]) return 3;
  else if(!itemType[0] && itemType[1] && itemType[2]) return 4;
  else if(itemType[0] && !itemType[1] && itemType[2]) return 5;
  else if(itemType[0] && itemType[1] && itemType[2]) return 6;

  return -1;
}

void PropertyDialog::createWidgets()
{
  idLabel = new QLabel(tr("ID:"));
  idLineEdit = new QLineEdit;
  idLabel->setBuddy(idLineEdit);
  QPalette *palette = new QPalette;
  palette->setColor(QPalette::Base, Qt::lightGray);
  palette->setColor(QPalette::Text, Qt::black);
  idLineEdit->setPalette(*palette);

  nameLabel = new QLabel(tr("Name:"));
  nameLineEdit = new QLineEdit;
  nameLabel->setBuddy(nameLineEdit);

  valueLabel = new QLabel(tr("Value:"));
  valueLineEdit = new QLineEdit;
  valueLabel->setBuddy(valueLineEdit);

  flowLabel = new QLabel(tr("Flow:"));
  flowLineEdit = new QLineEdit;
  flowLabel->setBuddy(flowLineEdit);

  capacityLabel = new QLabel(tr("Capacity:"));
  capacityLineEdit = new QLineEdit;
  capacityLabel->setBuddy(capacityLineEdit);

  fontSizeLabel = new QLabel(tr("Font Size:"));
  fontSizeComboBox = new QComboBox();
  for(int i = 10; i <= 60; i+= 2) {
    fontSizeComboBox->addItem(QString::number(i));
  }
  fontSizeLabel->setBuddy(fontSizeComboBox);

  widthLabel = new QLabel(tr("Width:"));
  widthLineEdit = new QLineEdit;
  widthLabel->setBuddy(widthLineEdit);

  sizeLabel = new QLabel(tr("Size:"));
  sizeLineEdit = new QLineEdit;
  sizeLabel->setBuddy(sizeLineEdit);

  colorLabel = new QLabel(tr("Color:"));
  colorComboBox = new QComboBox();
  QPixmap px(15, 15);
  QIcon icon;

  for(int i = 0; i < 4; i++) {
    px.fill(QColor(colorList[i]));
    icon.addPixmap(px);
    colorComboBox->addItem(icon, colorStrList[i]);
  }

  colorComboBox->addItem("others...");
  colorLabel->setBuddy(colorComboBox);
  
  cornerLabel = new QLabel(tr("Corner side:"));
  cornerComboBox = new QComboBox();
  cornerComboBox->addItem(QString("Bottom Right"));
  cornerComboBox->addItem(QString("Bottom Left"));
  cornerComboBox->addItem(QString("Top Right"));
  cornerComboBox->addItem(QString("Top Left"));

  buttonBox = new QDialogButtonBox();
  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);
}

void PropertyDialog::createLayouts()
{
  QGridLayout *gridLayout = new QGridLayout;

  gridLayout->addWidget(idLabel, 0, 0, Qt::AlignLeft);
  gridLayout->addWidget(idLineEdit, 0, 1, Qt::AlignLeft);
  gridLayout->addWidget(nameLabel, 1, 0, Qt::AlignLeft);
  gridLayout->addWidget(nameLineEdit, 1, 1, Qt::AlignRight);
  gridLayout->addWidget(valueLabel, 2, 0, Qt::AlignLeft);
  gridLayout->addWidget(valueLineEdit, 2, 1, Qt::AlignRight);
  gridLayout->addWidget(flowLabel, 3, 0, Qt::AlignLeft);
  gridLayout->addWidget(flowLineEdit, 3, 1, Qt::AlignRight); 
  gridLayout->addWidget(capacityLabel, 4, 0, Qt::AlignLeft);
  gridLayout->addWidget(capacityLineEdit, 4, 1, Qt::AlignRight);
  gridLayout->addWidget(fontSizeLabel, 5, 0, Qt::AlignLeft);
  gridLayout->addWidget(fontSizeComboBox, 5, 1, Qt::AlignRight);
  gridLayout->addWidget(colorLabel, 6, 0, Qt::AlignLeft);
  gridLayout->addWidget(colorComboBox, 6, 1, Qt::AlignRight);
  gridLayout->addWidget(widthLabel, 7, 0, Qt::AlignLeft);
  gridLayout->addWidget(widthLineEdit, 7, 1, Qt::AlignRight);
  gridLayout->addWidget(sizeLabel, 8, 0, Qt::AlignLeft);
  gridLayout->addWidget(sizeLineEdit, 8, 1, Qt::AlignRight);
  gridLayout->addWidget(cornerLabel, 9, 0, Qt::AlignLeft);
  gridLayout->addWidget(cornerComboBox, 9, 1, Qt::AlignRight);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(gridLayout);
  mainLayout->addWidget(buttonBox);

  setLayout(mainLayout);
}

void PropertyDialog::createConnections()
{
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(colorComboBox, SIGNAL(activated(int)),
	  this, SLOT(colorDialog(int)));
}

void PropertyDialog::colorDialog(int index)
{
  if(index == 4) color = QColorDialog::getColor(Qt::yellow, this);
  else color = colorList[index];
}
