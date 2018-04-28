#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "geneticalgorithmlayout.h"
#include "graphicsview.h"
#include "mainwindow.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"

#include <random>
#include <vector>
#include <map>
#include <iostream>
GeneticAlgorithmLayout::GeneticAlgorithmLayout(GraphicsView *view, QWidget *parent)
  : QUndoCommand(0)
{
  m_view = view;
  m_parent = parent;

  chromosomeLength = m_view->nodeId;
  EdgeItem *edge;
  Q_FOREACH(QGraphicsItem *item, view->scene()->items()) {
    if(item->type() == EdgeItemType) {
      edge = static_cast<EdgeItem*>(item);
      m_edges.push_back(Edge(edge->nodeU()->id(), edge->nodeV()->id(), edge->value()));
    }
    else if(item->type() == NodeItemType) {
      nodeList.push_back(static_cast<NodeItem*>(item));
    }
  }

  runFlag = false;
}

GeneticAlgorithmLayout::GeneticAlgorithmLayout(GraphicsView *view, long long int N,
					       std::vector< Edge > edges,
					       QWidget *parent)
  : QUndoCommand(0)
{
  m_view = view;
  m_parent = parent;
  m_edges = edges;
  runFlag = true;
  chromosomeLength = N;
}

void GeneticAlgorithmLayout::run(long long int g, long long int p,
				 double er, double cr, double mr)
{
  generation = g;
  population = p;
  eliteRate = er;
  crossRate = cr;
  crossCount = 2;
  mutateRate = mr;

  gridSize = 64;
  width = 1280 / gridSize;
  height = 720 / gridSize;

  chromosomes = std::vector< std::pair<double, std::vector< QPointF > > >(population);
  
  initialize();

  sort(chromosomes.rbegin(), chromosomes.rend(), pairCompare);
  for(int g = 0; g < generation; g++) {
    std::vector< std::pair<double, std::vector< QPointF > > > newChromosomes
      = selection(eliteRate);
    std::vector< std::pair<double, std::vector< QPointF > > > crossVec
      = crossover(crossRate, crossCount);
    std::vector< std::pair<double, std::vector< QPointF > > > mutateVec
      = mutate(mutateRate);
    
    newChromosomes.insert(newChromosomes.end(), crossVec.begin(), crossVec.end());
    newChromosomes.insert(newChromosomes.end(), mutateVec.begin(), mutateVec.end());

    chromosomes = newChromosomes;

    for(int p = eliteRate * population; p < population; p++) {
      chromosomes[p].first = calcFitness(p);
    }

    sort(chromosomes.rbegin(), chromosomes.rend(), pairCompare);
  }

  if(runFlag) {
    std::vector< NodeItem* > nodeTable(chromosomeLength, NULL);

    for(Edge e : m_edges) {
      if(e.from != e.to) {
	NodeItem *nodeU, *nodeV;
	if(nodeTable[e.from] == NULL) {
	  nodeU = new NodeItem(chromosomes[0].second[e.from], e.from);
	  nodeTable[e.from] = nodeU;
	  m_view->scene()->addItem(nodeU);
	  m_view->nodeId++;
	} else
	  nodeU = nodeTable[e.from];

	if(nodeTable[e.to] == NULL) {
	  nodeV = new NodeItem(chromosomes[0].second[e.to], e.to);
	  nodeTable[e.to] = nodeV;
	  m_view->scene()->addItem(nodeV);
	  m_view->nodeId++;
	} else
	  nodeV = nodeTable[e.to];

	EdgeItem *edge = new EdgeItem(chromosomes[0].second[e.from], 
				      chromosomes[0].second[e.to],
				      nodeU, nodeV, m_view->graphType());

	edge->setValue(e.cost);
	nodeU->addEdgeItem(edge);
	nodeV->addEdgeItem(edge);
	edge->setMultiTable(m_view->multipleEdgeTable());
	pll nodePair = GraphicsView::nodeItemPair(nodeU, nodeV);
	(*m_view->multipleEdgeTable())[nodePair].append(edge);

	m_view->scene()->addItem(edge);
      } else {
	NodeItem *node;

	if(nodeTable[e.from] == NULL) {
	  node = new NodeItem(chromosomes[0].second[e.from], e.from);
	  nodeTable[e.from] = node;
	  m_view->scene()->addItem(node);
	  m_view->nodeId++;
	} else
	  node = nodeTable[e.from];
	  
	LoopItem *loop = new LoopItem(node->position(), node, m_view->graphType());

	loop->setValue(e.cost);
	node->addLoopItem(loop);
	loop->setMultiTable(m_view->multipleLoopTable());
	(*m_view->multipleLoopTable())[node->id()].append(loop);

	m_view->scene()->addItem(loop);
      }
    }
  } else {
    oldPos = std::vector<QPointF>(m_view->nodeId);
    newPos = std::vector<QPointF>(m_view->nodeId);

    for(long long int i = 0; i < (long long int)chromosomes[0].second.size(); i++) {
      Q_FOREACH(NodeItem *v, nodeList) {
	if(v->id() == i) {
	  oldPos[v->id()] = v->pos();
	  v->setPos(chromosomes[0].second[i] - v->position());
	  newPos[v->id()] = chromosomes[0].second[i] - v->position();
	}
      }
    }
  }
}

void GeneticAlgorithmLayout::initialize()
{
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> randomX(0, width);
  std::uniform_int_distribution<int> randomY(0, height);

  int rx, ry;
  QPointF randPos;
  for(long long int p = 0; p < population; p++) {
    for(long long int i = 0; i < chromosomeLength; i++) {
      rx = randomX(mt) * gridSize; ry = randomY(mt) * gridSize;
      randPos = m_view->mapToScene(QPoint(rx, ry));
      chromosomes[p].second.push_back(randPos);
    }

    chromosomes[p].first = calcFitness(p);
  }
}

std::vector< std::pair<double, std::vector< QPointF > > > GeneticAlgorithmLayout::selection(double rate)
{
  std::vector< std::pair<double, std::vector< QPointF > > > select(population * rate);

  for(int i = 0; i < population * rate; i++) {
    select[i] = chromosomes[i];
  }

  return select;
}

std::vector< std::pair<double, std::vector< QPointF > > > GeneticAlgorithmLayout::crossover(double rate, int count)
{
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> choiceChromo(0, population - 1);
  std::uniform_int_distribution<int> choiceGene(0, chromosomeLength - 1);

  std::vector< std::pair<double, std::vector< QPointF > > > cross(2 * population * rate);

  for(int i = 0; i < population * rate; i++) {
    int chromo1 = choiceChromo(mt);
    int chromo2 = choiceChromo(mt);

    for(int j = 0; j < count; j++) {
      int gene = choiceGene(mt);

      auto tmp = (chromosomes[chromo1].second)[gene];
      (chromosomes[chromo1].second)[gene] = (chromosomes[chromo2].second)[gene];
      (chromosomes[chromo2].second)[gene] = tmp;
    }

    cross[2 * i] = chromosomes[chromo1];
    cross[2 * i + 1] = chromosomes[chromo2];
  }

  return cross;
}

std::vector< std::pair< double, std::vector< QPointF > > > GeneticAlgorithmLayout::mutate(double rate)
{
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> choiceChromo(0, population - 1);
  std::uniform_int_distribution<int> choiceGene(0, chromosomeLength - 1);
  std::uniform_int_distribution<int> randomX(0, width);
  std::uniform_int_distribution<int> randomY(0, height);

  std::vector< std::pair<double, std::vector< QPointF > > > muta(population * rate);

  for(int i = 0; i < population * rate; i++) {
    int chromo = choiceChromo(mt);
    int gene = choiceGene(mt);
    int rx = randomX(mt) * gridSize; int ry = randomY(mt) * gridSize;
    QPointF randPos = m_view->mapToScene(QPoint(rx, ry));

    muta[i] = chromosomes[chromo];
    (muta[i].second)[gene] = randPos;
  }

  return muta;
}

double GeneticAlgorithmLayout::calcFitness(long long int p)
{
  double res = 0.0;
  double maxLength = 0.0;

  for(long long int i = 0; i < (long long int)m_edges.size() - 1; i++) {
    for(long long int  j = i + 1; j < (long long int)m_edges.size(); j++) {
      res -= intersect((chromosomes[p].second)[m_edges[i].from],
		       (chromosomes[p].second)[m_edges[i].to],
		       (chromosomes[p].second)[m_edges[j].from],
		       (chromosomes[p].second)[m_edges[j].to]);
    }
    maxLength = std::max((chromosomes[p].second[m_edges[i].from] -
			  chromosomes[p].second[m_edges[i].to]).manhattanLength(),
			 maxLength);
  }
  maxLength = std::max((chromosomes[p].second[m_edges.back().from] - 
			chromosomes[p].second[m_edges.back().to]).manhattanLength(),
		       maxLength);

  res -= 0.01 * (maxLength == 0.0 ? 1e9 : maxLength);

  return res;
}

int GeneticAlgorithmLayout::intersect(QPointF p1, QPointF p2, QPointF p3, QPointF p4)
{
  double x1, x2, x3, x4, y1, y2, y3, y4;
  x1 = p1.x(); x2 = p2.x(); x3 = p3.x(); x4 = p4.x();
  y1 = p1.y(); y2 = p2.y(); y3 = p3.y(); y4 = p4.y();
  if(((x1 - x2) * (y3 - y1) + (y1 - y2) * (x1 - x3)) *
     ((x1 - x2) * (y4 - y1) + (y1 - y2) * (x1 - x4)) <= 0)
    if(((x3 - x4) * (y1 - y3) + (y3 - y4) * (x3 - x1)) *
       ((x3 - x4) * (y2 - y3) + (y3 - y4) * (x3 - x2)) <= 0)
      return 1;
  
  return 0;
}

void GeneticAlgorithmLayout::undo()
{
  Q_FOREACH(NodeItem *node, nodeList) {
    node->setPos(oldPos[node->id()]);
  }
}

void GeneticAlgorithmLayout::redo()
{
  Q_FOREACH(NodeItem *node, nodeList) {
    node->setPos(newPos[node->id()]);
  }
}
