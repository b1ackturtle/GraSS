#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "girth.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"
#include "graphicsview.h"

#include <vector>
#include <queue>
#include <iostream>

Girth::Girth(const GraphData &graph, QWidget *parent)
  : Calc(graph, parent)
{
  tabTitle = tr("Girth Result");
}

void Girth::calc()
{
  GraphicsView *newView = createTab(true);

  std::vector< std::vector< Edge > > graph = graphData->listGraph();
  long long int N = graphData->nodeSize();
  
  double res = std::numeric_limits<double>::infinity();
  std::vector< long long int > path;
  std::vector< std::vector< double > > dist = graphData->matrixGraph();

  for(long long int v = 0; v < N; v++) {
    std::vector< bool > visited(N, false);
    std::queue< long long int > Q;
    std::vector< long long int > parent(N, -1);
    std::vector< double > D = dist[v];

    Q.push(v);

    while(!Q.empty()) {
      long long int q = Q.front(); Q.pop();
      visited[q] = true;
      for(auto e : graph[q]) {
	if(e.to == parent[q]) continue;
	if(!visited[e.to]) {
	  parent[e.to] = q;
	  D[e.to] = D[q] + dist[q][e.to];
	  Q.push(e.to);
	} else {
	  if(res > D[q] + D[e.to] + dist[q][e.to]) {
	    res = D[q] + D[e.to] + dist[q][e.to];
	    path.clear();
	    path.push_back(v);
	    if(v != e.to) path.push_back(e.to);
	    for(long long int p = q; p != v; p = parent[p]) {
	      path.push_back(p);
	    }
	    path.push_back(v);
	  }
	}
      }
    }
  }

  std::vector< std::vector< bool > > flag(N, std::vector< bool >(N, false));
  for(unsigned int i = 0; i < path.size()-1; i++) {
    flag[path[i]][path[i+1]] = flag[path[i+1]][path[i]] = true;
  }
  Q_FOREACH(QGraphicsItem *item, newView->scene()->items()) {
    if(item->type() == EdgeItemType) {
      EdgeItem *edge = static_cast<EdgeItem*>(item);
      long long int id1 = graphData->smallNodeMapedID[edge->nodeU()->id()];
      long long int id2 = graphData->smallNodeMapedID[edge->nodeV()->id()];
      if(flag[id1][id2]) edge->setColor(Qt::red);
    }
  }

  resultDialogShow(res, path);
}

void Girth::resultDialogShow(const double result,
			     const std::vector< long long int > &path)
{
  resultDialog = new QDialog(m_parent);
  resultDialog->setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  QHBoxLayout *res = new QHBoxLayout;
  res->addWidget(new QLabel(tr("Result:")));
  res->addWidget(new QLabel(QString::number(result)));
  QHBoxLayout *pathLayout = new QHBoxLayout;
  pathLayout->addWidget(new QLabel("Path:"));
  pathLayout->addWidget(new QLabel(QString::number(graphData->bigNodeMapedID[path[0]])));
  for(unsigned int i = 1; i < path.size(); i++) {
    pathLayout->addWidget(new QLabel("--->"));
    pathLayout->addWidget(new QLabel(QString::number(graphData->bigNodeMapedID[path[i]])));
  }
  QHBoxLayout *btn = new QHBoxLayout;
  btn->addStretch();
  QPushButton *okButton = new QPushButton(tr("OK"));
  connect(okButton, SIGNAL(clicked()), resultDialog, SLOT(accept()));
  btn->addWidget(okButton);
  btn->addStretch();
  mainLayout->addLayout(res);
  mainLayout->addLayout(pathLayout);
  mainLayout->addLayout(btn);
  resultDialog->setLayout(mainLayout);
  resultDialog->show();
}
