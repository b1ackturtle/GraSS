#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "tspga.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"
#include "graphicsview.h"

#include <vector>
#include <map>
#include <random>

TspGA::TspGA(const GraphData &graph, QWidget *parent)
  : Calc(graph, parent)
{
  tabTitle = tr("Genetic Algorithm Result");

  createWidgets();
  createLayouts();
  createConnects();
}

void TspGA::createWidgets()
{
  startIDLabel = new QLabel(tr("Start ID:"));
  startID = new QComboBox;
  startID->setMaximumWidth(50);
  startIDLabel->setBuddy(startID);

  for(unsigned int i = 0; i < graphData->existID.size(); i++) {
    if(graphData->existID[i]) {
      QString idx = QString::number(i);
      startID->addItem(idx);
    }
  }

  cycleCheckLabel = new QLabel(tr("Cycle:"));
  cycleCheckBox = new QCheckBox;

  buttonBox = new QDialogButtonBox;
  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);
}

void TspGA::createLayouts()
{
  QHBoxLayout *idLayout = new QHBoxLayout;
  idLayout->addWidget(startIDLabel);
  idLayout->addWidget(startID);

  idLayout->addWidget(cycleCheckLabel);
  idLayout->addWidget(cycleCheckBox);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(idLayout);
  mainLayout->addWidget(buttonBox);

  setLayout(mainLayout);
}

void TspGA::createConnects()
{
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(calc()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void TspGA::calc()
{
  GraphicsView *newView = createTab(true);

  graph = graphData->matrixGraph();
  N = graphData->nodeSize();
  start = graphData->smallNodeMapedID[startID->currentText().toLong()];

  std::vector< long long int > path;
  double res = genetic(N, 30, 0.2, 0.2, 500, path);

  std::vector< std::vector< bool > > pathTable(N, std::vector< bool >(N, false));
  for(int i = 0; i < path.size() - 1; i++) {
    pathTable[path[i]][path[i+1]] = pathTable[path[i+1]][path[i]] = true;
  }
  Q_FOREACH(QGraphicsItem *i, newView->scene()->items()) {
    if(i->type() == EdgeItemType) {
      EdgeItem *edge = static_cast<EdgeItem*>(i);
      long long int id1 = graphData->smallNodeMapedID[edge->nodeU()->id()];
      long long int id2 = graphData->smallNodeMapedID[edge->nodeV()->id()];
      if(pathTable[id1][id2])
	edge->setColor(Qt::red);
    }
  }

  resultDialogShow(res, path);
}

double TspGA::genetic(long long int popnum, int popsize, double mutprob, double elite,
		      int mixture, std::vector< long long int > &path)
{
  std::random_device rd;
  std::uniform_int_distribution<int> randint(0, popnum-1);
  std::uniform_real_distribution<double> randdouble(0, 1.0);
  std::vector< std::vector< long long int > > pop;
  std::vector< std::pair< double, std::vector< long long int > > > scores;
  int topElite = int(elite * popsize);

  std::vector< long long int > tmp;
  for(int i = 0; i < popnum; i++) {
    if(i == start) continue;
    tmp.push_back(i);
  }

  for(int i = 0; i < popsize; i++) {
    next_permutation(tmp.begin(), tmp.end());
    std::vector< long long int > x;
    x.push_back(start);
    for(int j = 0; j < tmp.size(); j++) x.push_back(tmp[j]);
    if(cycleCheckBox->checkState()) x.push_back(start);
    pop.push_back(x);
  }

  for(int i = 0; i < mixture; i++) {
    for(int j = 0; j < pop.size(); j++) {
      scores.push_back(std::make_pair(costf(pop[j]), pop[j]));
    }
    sort(scores.begin(), scores.end());

    pop.clear();
    for(int k = 0; k < topElite; k++) {
      pop.push_back(scores[k].second);
    }

    std::uniform_int_distribution< int > randElite(0, topElite);
    while(pop.size() < popsize) {
      if(randdouble(rd) < mutprob) {
	int c = randElite(rd);
	pop.push_back(mutate(scores[c].second, popnum));
      } else {
	int c1 = randElite(rd);
	int c2 = randElite(rd);
	pop.push_back(crossover(scores[c1].second, scores[c2].second, popnum));
      }
    }
  }

  path = scores[0].second;
  return scores[0].first;
}

std::vector< long long int > TspGA::mutate(std::vector< long long int > vec, int popnum)
{
  std::random_device rd;
  std::uniform_int_distribution< int > randInt(1, popnum-1);
  int t = randInt(rd);
  int s = randInt(rd);
  std::swap(vec[t], vec[s]);
  return vec;
}

std::vector< long long int > TspGA::crossover(std::vector< long long int > r1,
					      std::vector< long long int > r2, int n)
{
  std::random_device rd;
  std::vector< long long int > res;
  std::uniform_int_distribution< int > randInt(1, n - 1);
  int t = randInt(rd);

  for(int i = 0; i < t; i++) {
    res.push_back(r1[i]);
  }
  for(int i = 0; i < r2.size(); i++) {
    bool flag = false;
    for(int j = 0; j < res.size(); j++) {
      if(r2[i] == res[j]) {flag = true; break;}
    }
    if(!flag) {
      res.push_back(r2[i]);
    }
  }
  if(cycleCheckBox->checkState()) res.push_back(start);

  return res;
}

double TspGA::costf(const std::vector< long long int > &vec)
{
  double res = 0;

  for(int i = 0; i < vec.size() - 1; i++) {
    res += graph[vec[i]][vec[i+1]];
  }

  return res;
}

void TspGA::resultDialogShow(double result, std::vector< long long int > &path)
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
  for(int i = 1; i < path.size(); i++) {
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
