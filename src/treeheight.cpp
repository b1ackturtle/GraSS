#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "treeheight.h"

#include <vector>
#include <queue>

TreeHeight::TreeHeight(const GraphData &graph, QWidget *parent)
  : Calc(graph, parent)
{
}

void TreeHeight::calc()
{
  graph = graphData->listGraph();
  
  std::vector< double > res = height();
  
  resultDialogShow(res);
}

std::vector< double > TreeHeight::height()
{
  N = graphData->nodeSize();
  std::vector< double > ht(N);
  std::vector< double > v1(N, std::numeric_limits< double >::infinity());
  std::vector< double > v2(N, std::numeric_limits< double >::infinity());
  std::vector< double > v3(N, std::numeric_limits< double >::infinity());

  dist(0, v1);
  int x = std::max_element(v1.begin(), v1.end()) - v1.begin();
  dist(x, v2);
  int y = std::max_element(v2.begin(), v2.end()) - v2.begin();
  dist(y, v3);
  for(int i = 0; i < N; i++) {
    ht[i] = std::max(v2[i], v3[i]);
  }

  return ht;
}

void TreeHeight::dist(long long int u, std::vector< double >& vec)
{
  std::queue< long long int > Q;
  Q.push(u);
  vec[u] = 0;
  while(!Q.empty()) {
    int v = Q.front(); Q.pop();
    for(int i = 0; i < graph[v].size(); i++) {
      if(vec[graph[v][i].to] > vec[v] + graph[v][i].cost) {
	vec[graph[v][i].to] = vec[v] + graph[v][i].cost;
	Q.push(graph[v][i].to);
      }
    }
  }
}
    
void TreeHeight::resultDialogShow(const std::vector< double > ht)
{
  resultDialog = new QDialog(m_parent);
  resultDialog->setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  QVBoxLayout *res = new QVBoxLayout;
  QTableWidget *table = new QTableWidget;
  table->setColumnCount(2);
  table->setRowCount(ht.size());
  table->setHorizontalHeaderLabels(QStringList() << tr("Root Node ID") << tr("Height"));
  table->horizontalHeader()->setStretchLastSection(true);
  for(int i = 0; i < ht.size(); i++) {
    QTableWidgetItem *nodeID = new QTableWidgetItem(QString::number(i));
    QTableWidgetItem *height = new QTableWidgetItem(QString::number(ht[i]));
    nodeID->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    height->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    table->setItem(i, 0, nodeID);
    table->setItem(i, 1, height);
  }

  res->addWidget(new QLabel(tr("Result:")));
  res->addWidget(table);
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
