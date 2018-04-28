#ifndef GENETICALGORITHMLAYOUT_H
#define GENETICALGORITHMLAYOUT_H

#include <QUndoCommand>

#include "graphdata.hpp"

class GraphicsView;
class NodeItem;

class GeneticAlgorithmLayout : public QUndoCommand
{
public:
  GeneticAlgorithmLayout(GraphicsView *view, QWidget *parent = 0);
  GeneticAlgorithmLayout(GraphicsView *view, long long int N, std::vector< Edge > edges,
			 QWidget *parent = 0);

  void run(long long int g, long long int p, double er, double cr, double mr);
  void undo();
  void redo();

private:
  void initialize();
  std::vector< std::pair<double, std::vector< QPointF > > > selection(double rate);
  std::vector< std::pair<double, std::vector< QPointF > > > crossover(double rate,
								      int count);
  std::vector< std::pair<double, std::vector< QPointF > > > mutate(double rate);
  double calcFitness(long long int p);
  int intersect(QPointF p1, QPointF p2, QPointF p3, QPointF p4);

  static bool pairCompare(const std::pair<double, std::vector< QPointF > >& firstElem,
			  const std::pair<double, std::vector< QPointF > >& secondElem) {
    return firstElem.first < secondElem.first;
  }

  GraphicsView *m_view;
  QWidget *m_parent;
  std::vector< Edge > m_edges;
  QList< NodeItem* > nodeList;
  bool runFlag;

  std::vector< std::pair<double, std::vector< QPointF > > > chromosomes;

  int gridSize;
  int width;
  int height;

  long long int generation;
  long long int chromosomeLength;
  long long int population;
  double eliteRate;
  double crossRate;
  int crossCount;
  double mutateRate;

  std::vector<QPointF> oldPos;
  std::vector<QPointF> newPos;
};

#endif
