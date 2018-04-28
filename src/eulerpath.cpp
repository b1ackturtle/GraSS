#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "eulerpath.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"
#include "graphicsview.h"

#include <iostream>

EulerPath::EulerPath(const GraphData &graph, QWidget *parent)
  : Calc(graph, parent)
{
  createWidgets();
  createLayouts();
  createConnects();
}

void EulerPath::calc()
{
  std::vector< std::vector< Edge > > graph = graphData->listGraph();
  bool isEuler = true;
  long long int N = graphData->nodeSize();
  std::vector< long long int > outNodeCnt(N, 0), inNodeCnt(N, 0);
  
  if(cycleCheckBox->checkState()) {
    if(graphData->graphType() == UNDIRECTED) {
      for(long long int i = 0; i < N; i++) {
	if(graph[i].size() % 2 != 0) {
	  isEuler = false;
	  break;
	}
      }
    } else {
      for(long long int i = 0; i < N; i++) {
	outNodeCnt[i] = graph[i].size();
	for(auto e : graph[i]) {
	  inNodeCnt[e.to]++;
	}
      }
      for(long long int i = 0; i < N; i++) {
	if(outNodeCnt[i] - inNodeCnt[i] != 0) {
	  isEuler = false;
	  break;
	}
      }
    }

    resultDialogShow(isEuler);
  } else {
    std::vector< long long int > checkId = {-1, -1};
    if(graphData->graphType() == UNDIRECTED) {
      for(long long int i = 0; i < N; i++) {
	if(graph[i].size() % 2 != 0) {
	  checkId.push_back(i);
	}
      }
      if(checkId.size() - 2 > 2) isEuler = false;
    } else {
      std::vector< long long int > startId, endId;
      for(long long int i = 0; i < N; i++) {
	outNodeCnt[i] = graph[i].size();
	for(auto e : graph[i]) {
	  inNodeCnt[e.to]++;
	}
      }
      for(long long int i = 0; i < N; i++) {
	if(outNodeCnt[i] - inNodeCnt[i] == 1) startId.push_back(i);
	if(inNodeCnt[i] - outNodeCnt[i] == 1) endId.push_back(i);
      }
      if(startId.size() != 1 || endId.size() != 1) isEuler = false;
      else {
	checkId.push_back(startId.front());
	checkId.push_back(endId.front());
      }
    }

    resultDialogShow(isEuler, 
		     graphData->bigNodeMapedID[checkId[checkId.size()-2]],
		     graphData->bigNodeMapedID[checkId.back()]);
  }
}

void EulerPath::createWidgets()
{
  cycleCheckLabel = new QLabel(tr("Cycle:"));
  cycleCheckBox = new QCheckBox;

  buttonBox = new QDialogButtonBox;
  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);
}

void EulerPath::createLayouts()
{
  QHBoxLayout *checkLayout = new QHBoxLayout;
  checkLayout->addWidget(cycleCheckLabel);
  checkLayout->addWidget(cycleCheckBox);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(checkLayout);
  mainLayout->addWidget(buttonBox);

  setLayout(mainLayout);
}

void EulerPath::createConnects()
{
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(calc()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void EulerPath::resultDialogShow(const bool isEuler)
{
  resultDialog = new QDialog(m_parent);
  resultDialog->setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  QHBoxLayout *res = new QHBoxLayout;
  res->addWidget(new QLabel(tr("Is Euler Cycle:")));
  QLabel *euler = (isEuler) ? (new QLabel(tr("True"))) : (new QLabel(tr("False")));
  res->addWidget(euler);
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

void EulerPath::resultDialogShow(const bool isEuler, 
				 const long long int start, const long long int end)
{
  resultDialog = new QDialog(m_parent);
  resultDialog->setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  QHBoxLayout *res = new QHBoxLayout;
  res->addWidget(new QLabel(tr("Is Euler Path:")));
  QLabel *euler = (isEuler) ? (new QLabel(tr("True"))) : (new QLabel(tr("False")));
  res->addWidget(euler);
  QHBoxLayout *startLayout = new QHBoxLayout;
  QHBoxLayout *endLayout = new QHBoxLayout;
  if(isEuler) {
    startLayout->addWidget(new QLabel(tr("Start ID:")));
    startLayout->addWidget(new QLabel(QString::number(start)));
    endLayout->addWidget(new QLabel(tr("End ID:")));
    endLayout->addWidget(new QLabel(QString::number(end)));
  }
  QHBoxLayout *btn = new QHBoxLayout;
  btn->addStretch();
  QPushButton *okButton = new QPushButton(tr("OK"));
  connect(okButton, SIGNAL(clicked()), resultDialog, SLOT(accept()));
  btn->addWidget(okButton);
  btn->addStretch();
  mainLayout->addLayout(res);
  mainLayout->addLayout(startLayout);
  mainLayout->addLayout(endLayout);
  mainLayout->addLayout(btn);
  resultDialog->setLayout(mainLayout);
  resultDialog->show();
}
