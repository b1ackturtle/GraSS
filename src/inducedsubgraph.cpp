#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "inducedsubgraph.h"
#include "graphicsview.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"

#include <vector>
#include <sstream>
#include <string>
#include <iostream>

InducedSubgraph::InducedSubgraph(const GraphData &graph, QWidget *parent)
  : Calc(graph, parent)
{
  tabTitle = tr("Induced Subgraph Result");

  deleteFlag = std::vector< bool >(graphData->nodeSize(), true);

  createWidgets();
  createLayouts();
  createConnects();
}

void InducedSubgraph::calc()
{
  GraphicsView *newView = createTab(true);

  Q_FOREACH(QGraphicsItem *i, newView->scene()->items()) {
    if(i->type() == EdgeItemType) {
      EdgeItem *edge = static_cast<EdgeItem*>(i);
      long long int id1 = edge->nodeU()->id(), id2 = edge->nodeV()->id();
      if(deleteFlag[id1] || deleteFlag[id2]) {
	pll nodePair = GraphicsView::nodeItemPair(edge->nodeU(), edge->nodeV());
	(*newView->multipleEdgeTable())[nodePair].removeAt(edge->multiTableIndex());
	edge->nodeU()->removeAllEdgeList(edge);
	edge->nodeV()->removeAllEdgeList(edge);

	if(deleteFlag[id1] && !edge->nodeU()->edgeList().size())
	  newView->scene()->removeItem(edge->nodeU());
	if(deleteFlag[id2] && !edge->nodeV()->edgeList().size())
	  newView->scene()->removeItem(edge->nodeV());

	newView->scene()->removeItem(edge);
      }
    }
    else if(i->type() == NodeItemType) {
      NodeItem *node = static_cast<NodeItem*>(i);
      if(!node->edgeList().size() && deleteFlag[node->id()]) {
	newView->scene()->removeItem(node);
      }
    }
  }
}

void InducedSubgraph::enableOkButton()
{
  fill(deleteFlag.begin(), deleteFlag.end(), true);

  if(setNodesLineEdit->text().isEmpty())
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  else {
    std::string setNodes = setNodesLineEdit->text().toLocal8Bit().constData();
    std::vector< std::string > nodes = split(setNodes, ',');
    bool flag = true;
    for(auto node : nodes) {
      if(node == "") continue;
      
      if(std::stoi(node) >= graphData->nodeSize()) {
	flag = false;
	break;
      }
      else if(!graphData->existID[std::stoi(node)]) {
	flag = false;
	break;
      }
      else {
	deleteFlag[graphData->bigNodeMapedID[std::stoi(node)]] = false;
      }
    }

    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(flag);
  }
}

void InducedSubgraph::createWidgets()
{
  setNodesLabel = new QLabel(tr("Set of nodes:"));

  setNodesLineEdit = new QLineEdit;
  QRegExp rx("[0-9,]{1,128}");
  setNodesLineEdit->setValidator(new QRegExpValidator(rx, this));
  setNodesLabel->setBuddy(setNodesLineEdit);

  buttonBox = new QDialogButtonBox;
  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void InducedSubgraph::createLayouts()
{
  QHBoxLayout *setNodesLayout = new QHBoxLayout;
  setNodesLayout->addWidget(setNodesLabel);
  setNodesLayout->addWidget(setNodesLineEdit);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(setNodesLayout);
  mainLayout->addWidget(buttonBox);

  setLayout(mainLayout);
}

void InducedSubgraph::createConnects()
{
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(calc()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  connect(setNodesLineEdit, SIGNAL(textEdited(const QString)),
	  this, SLOT(enableOkButton()));
}

std::vector< std::string > InducedSubgraph::split(const std::string &input, char delimiter)
{
  std::istringstream stream(input);

  std::string field;
  std::vector< std::string > result;
  while(std::getline(stream, field, delimiter)) {
    result.push_back(field);
  }

  return result;
}
