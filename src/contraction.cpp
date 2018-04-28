#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "contraction.h"
#include "graphicsview.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"

#include <vector>
#include <sstream>
#include <string>
#include <iostream>

Contraction::Contraction(const GraphData &graph, QWidget *parent)
  : Calc(graph, parent)
{
  tabTitle = tr("Contraction Result");

  deleteEdges.clear();

  selectableEdges = graphData->matrixGraph();

  createWidgets();
  createLayouts();
  createConnects();
}

void Contraction::calc()
{
  GraphicsView *newView = createTab(true);

  for(auto delEdge : deleteEdges) {
    long long int newNodeId = contraction(delEdge.first, delEdge.second, newView);

    for(auto tmp : deleteEdges) {
      if(delEdge == tmp) continue;
      if(tmp.first == delEdge.first || tmp.first == delEdge.second)
	tmp.first = newNodeId;
      else if(tmp.second == delEdge.first || tmp.second == delEdge.second)
	tmp.second = newNodeId;
    }
  }
}

int Contraction::contraction(int edge1, int edge2, GraphicsView *view)
{
  NodeItem *node1, *node2;
  std::vector< NodeItem* > readdEdges;

  Q_FOREACH(QGraphicsItem *i, view->scene()->items()) {
    if(i->type() == EdgeItemType) {
      EdgeItem *edge = static_cast<EdgeItem*>(i);
      long long int id1 = edge->nodeU()->id(), id2 = edge->nodeV()->id();
      if(edge1 == id1 || edge1 == id2 || edge2 == id1 || edge2 == id2) {
	pll nodePair = GraphicsView::nodeItemPair(edge->nodeU(), edge->nodeV());
	(*view->multipleEdgeTable())[nodePair].removeAt(edge->multiTableIndex());
	edge->nodeU()->removeAllEdgeList(edge);
	edge->nodeV()->removeAllEdgeList(edge);

	if((edge1 == id1 && edge2 != id2) || (edge2 == id1 && edge1 != id2))
	  readdEdges.push_back(edge->nodeV());
	if((edge1 == id2 && edge2 != id1) || (edge2 == id2 && edge1 != id1))
	  readdEdges.push_back(edge->nodeU());

	view->scene()->removeItem(edge);
      }
    }
    else if(i->type() == NodeItemType) {
      NodeItem *node = static_cast<NodeItem*>(i);
      if(node->id() == edge1) node1 = node;
      else if(node->id() == edge2) node2 = node;
    }
  }

  int ret = view->nodeId;
  NodeItem *node = new NodeItem((node1->position()+node2->position())/2, view->nodeId++);
  view->scene()->removeItem(node1);
  view->scene()->removeItem(node2);
  view->scene()->addItem(node);

  EdgeItem *edge;
  for(auto r : readdEdges) {
    edge = new EdgeItem(r->position(), node->position(), r, node, view->graphType());
    edge->setMultiTable(view->multipleEdgeTable());
    r->addEdgeItem(edge);
    node->addEdgeItem(edge);

    pll nodePair = GraphicsView::nodeItemPair(r, node);
    (*view->multipleEdgeTable())[nodePair].append(edge);

    view->scene()->addItem(edge);
  }

  return ret;
}

void Contraction::enableOkButton()
{
  deleteEdges.clear();

  if(setEdgesLineEdit->text().isEmpty())
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  else {
    std::string setEdges = setEdgesLineEdit->text().toLocal8Bit().constData();
    if(setEdges[0] == '{' && setEdges.back() == '}') {
      setEdges.pop_back();
      std::vector< std::string > edges = split(setEdges, ')');
    
      bool flag = true;
      for(auto edge : edges) {
	edge.erase(0, 2);
	std::vector< std::string > e = split(edge, ',');
	
	if(e.size() != 2) {flag = false; break;}
	else if(e[0] == "" || e[1] == "") continue;
	else if(std::stoi(e[0]) >= graphData->nodeSize() || 
		std::stoi(e[1]) >= graphData->nodeSize()) {
	  flag = false;
	  break;
	}
	else if(!graphData->existID[std::stoi(e[0])] ||
		!graphData->existID[std::stoi(e[1])]) {
	  flag = false;
	  break;
	}
	else if(isinf(selectableEdges[std::stoi(e[0])][std::stoi(e[1])]) && 
		isinf(selectableEdges[std::stoi(e[0])][std::stoi(e[1])])) {
	  flag = false;
	  break;
	}
	else {
	  deleteEdges.push_back(std::make_pair(graphData->bigNodeMapedID[std::stoi(e[0])],
					       graphData->bigNodeMapedID[std::stoi(e[1])]));
	}
      }
      buttonBox->button(QDialogButtonBox::Ok)->setEnabled(flag);
    } else {
      buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
  }
}

void Contraction::createWidgets()
{
  setEdgesLabel = new QLabel(tr("Set of edges:"));

  setEdgesLineEdit = new QLineEdit;
  QRegExp rx("[0-9,{}()]{1,128}");
  setEdgesLineEdit->setValidator(new QRegExpValidator(rx, this));
  setEdgesLabel->setBuddy(setEdgesLineEdit);

  buttonBox = new QDialogButtonBox;
  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void Contraction::createLayouts()
{
  QHBoxLayout *setEdgesLayout = new QHBoxLayout;
  setEdgesLayout->addWidget(setEdgesLabel);
  setEdgesLayout->addWidget(setEdgesLineEdit);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(setEdgesLayout);
  mainLayout->addWidget(buttonBox);

  setLayout(mainLayout);
}

void Contraction::createConnects()
{
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(calc()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  connect(setEdgesLineEdit, SIGNAL(textEdited(const QString)),
	  this, SLOT(enableOkButton()));
}

std::vector< std::string > Contraction::split(const std::string &input, char delimiter)
{
  std::istringstream stream(input);

  std::string field;
  std::vector< std::string > result;
  while(std::getline(stream, field, delimiter)) {
    result.push_back(field);
  }

  return result;
}
