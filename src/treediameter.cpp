#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "treediameter.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"
#include "graphicsview.h"

#include <vector>
#include <queue>

TreeDiameter::TreeDiameter(const GraphData &graph, QWidget *parent)
  : Calc(graph, parent)
{
  tabTitle = tr("Tree Diameter Result");
}

void TreeDiameter::calc()
{
  GraphicsView *newView = createTab(true);

  graph = graphData->listGraph();
  N = graphData->nodeSize();

  std::pair< double, long long int > s = dist(-1, 0);
  std::pair< double, long long int > t = dist(-1, s.second);

  std::vector< long long int > tmp; tmp.push_back(s.second);
  std::vector< long long int > res = path(t.second, tmp, std::vector< bool >(N, false));
  std::vector< std::vector< bool > > pathFlag(N, std::vector< bool >(N, false));
  for(unsigned int i = 0; i < res.size() - 1; i++) {
    pathFlag[res[i]][res[i+1]] = pathFlag[res[i+1]][res[i]] = true;
  }
  Q_FOREACH(QGraphicsItem *i, newView->scene()->items()) {
    if(i->type() == EdgeItemType) {
      EdgeItem *edge = static_cast<EdgeItem*>(i);
      long long int id1 = graphData->smallNodeMapedID[edge->nodeU()->id()];
      long long int id2 = graphData->smallNodeMapedID[edge->nodeV()->id()];
      if(pathFlag[id1][id2])
	edge->setColor(Qt::red);
    }
  }

  resultDialogShow(t.first,
		   graphData->bigNodeMapedID[s.second],
		   graphData->bigNodeMapedID[t.second]);
}

std::pair< double, long long int > TreeDiameter::dist(long long int u, long long int v)
{
  std::pair< double, long long int > res(0, v);
  for(auto e : graph[v]) {
    if(e.to != u) {
      std::pair< double, long long int > tmp = dist(v, e.to);
      tmp.first += e.cost;
      if(res.first < tmp.first) res = tmp;
    }
  }

  return res;
}

std::vector< long long int > TreeDiameter::path(long long int t,
						std::vector< long long int > p,
						std::vector< bool > visited)
{
  if(p.back() == t) return p;
  for(auto e : graph[p.back()]) {
    if(visited[e.to]) continue;
    std::vector< long long int > q(p); q.push_back(e.to);
    std::vector< bool > v(visited); v[e.to] = true;
    std::vector< long long int > res = path(t, q, v);
    if(res.back() == t) return res;
  }

  return p;
}

void TreeDiameter::resultDialogShow(const double result,
				    const long long int x, const long long int y)
{
  resultDialog = new QDialog(m_parent);
  resultDialog->setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  QHBoxLayout *res = new QHBoxLayout;
  res->addWidget(new QLabel(tr("Result:")));
  res->addWidget(new QLabel(QString::number(result)));
  QHBoxLayout *pair = new QHBoxLayout;
  pair->addWidget(new QLabel(tr("Farthest node pair:")));
  pair->addWidget(new QLabel("(" + QString::number(x) + "," + QString::number(y) + ")"));
  QHBoxLayout *btn = new QHBoxLayout;
  btn->addStretch();
  QPushButton *okButton = new QPushButton(tr("OK"));
  connect(okButton, SIGNAL(clicked()), resultDialog, SLOT(accept()));
  btn->addWidget(okButton);
  btn->addStretch();
  mainLayout->addLayout(res);
  mainLayout->addLayout(pair);
  mainLayout->addLayout(btn);
  resultDialog->setLayout(mainLayout);
  resultDialog->show();
}
