#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "prim.h"
#include "graphicsview.h"
#include "graphdata.hpp"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"

#include <vector>
#include <queue>

Prim::Prim(const GraphData &graph, QWidget *parent)
  : Calc(graph, parent)
{
  tabTitle = tr("Prim Result");
}

void Prim::calc()
{
  GraphicsView *newView = createTab(true);

  graph = graphData->matrixGraph();
  long long int N = graphData->nodeSize();
  
  //Prim's Algorithm
  std::vector< Edge > T;
  double total = 0.0;

  std::vector< bool > visited(N);
  auto comp = [](Edge a, Edge b) {return a.cost > b.cost;};
  std::priority_queue< Edge, std::vector<Edge>, decltype(comp) > Q(comp);
  Q.push(Edge(-1, 0, 0));
  while(!Q.empty()) {
    Edge e = Q.top(); Q.pop();
    if(visited[e.to]) continue;
    T.push_back(e);
    total += e.cost;
    visited[e.to] = true;
    
    for(long long int i = 0; i < (long long int)graph[e.to].size(); i++) {
      if(graph[e.to][i] != std::numeric_limits<double>::infinity() &&
	 graph[e.to][i] != std::numeric_limits<double>::quiet_NaN() && !visited[i]) {
	Q.push(Edge(e.to, i, graph[e.to][i]));
      }
    }
  }

  //Draw Minimum Spanning Tree
  Q_FOREACH(QGraphicsItem *i, newView->scene()->items()) {
    if(i->type() == EdgeItemType) {
      EdgeItem *edge = static_cast<EdgeItem*>(i);
      edge->setColor(Qt::black);
      long long int id1 = edge->nodeU()->id(), id2 = edge->nodeV()->id();
      bool deleteFlag = true;
      for(auto it = T.begin(); it != T.end(); ++it) {
	long long int p1 = graphData->bigNodeMapedID[(*it).from];
	long long int p2 = graphData->bigNodeMapedID[(*it).to];
	if((id1 == p1 && id2 == p2) || (id1 == p2 && id2 == p1))
	  deleteFlag = false;
      }
      if(deleteFlag) {
	pll nodePair = GraphicsView::nodeItemPair(edge->nodeU(), edge->nodeV());
	(*newView->multipleEdgeTable())[nodePair].removeAt(edge->multiTableIndex());
	edge->nodeU()->removeAllEdgeList(edge);
	edge->nodeV()->removeAllEdgeList(edge);
	
	newView->scene()->removeItem(edge);
      }
    }
  }

  result = total;
  resultDialogShow();
}

void Prim::resultDialogShow()
{
  resultDialog = new QDialog(m_parent);
  resultDialog->setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  QHBoxLayout *res = new QHBoxLayout;
  res->addWidget(new QLabel(tr("Result:")));
  res->addWidget(new QLabel(QString::number(result)));
  QHBoxLayout *btn = new QHBoxLayout;
  btn->addStretch();
  QPushButton *okButton = new QPushButton(tr("OK"));
  connect(okButton, SIGNAL(clicked()), resultDialog, SLOT(accept()));
  btn->addWidget(okButton);
  btn->addStretch();
  mainLayout->addLayout(res);
  mainLayout->addLayout(btn);
  resultDialog->setLayout(mainLayout);
  resultDialog->show();
}
