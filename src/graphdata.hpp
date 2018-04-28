#ifndef GRAPHDATA_HPP
#define GRAPHDATA_HPP

#include "tools.hpp"

class QGraphicsItem;
class GraphicsView;

struct Edge {
  long long int from;
  long long int to;
  double cost;
  double cap;
  Edge(long long int from, long long int to, double cost, double cap)
    : from(from), to(to), cost(cost), cap(cap) {}
  Edge(long long int from, long long int to, double cost)
    : from(from), to(to), cost(cost), cap(std::numeric_limits<double>::quiet_NaN()) {}
};

struct Path {
  long long int to;
  double cap;
  double cost;
  double rev;
  Path(long long int to, double cap, double rev)
    : to(to), cap(cap), cost(std::numeric_limits<double>::quiet_NaN()), rev(rev) {}
  Path(long long int to, double cap, double cost, double rev)
    : to(to), cap(cap), cost(cost), rev(rev) {}
};

class GraphData
{
public:
  explicit GraphData(GraphicsView *view);
  explicit GraphData(long long int N, const std::vector< Edge > &edges);
  ~GraphData();

  std::vector< std::vector<double> > matrixGraph() const {return m_matrixGraph;}
  std::vector< std::vector<Edge> > listGraph() const {return m_listGraph;}
  long long int nodeSize() const {return N;}
  GraphType graphType() const {return m_graphType;}

  bool hasNan() const {return flagNan;}
  bool hasNeg() const {return flagNeg;}
  bool hasNanCap() const {return flagNanCap;}
  bool hasNegCap() const {return flagNegCap;}
  bool hasNanFlow() const {return flagNanFlow;}
  bool hasNegFlow() const {return flagNegFlow;}

  bool isBipartite();
  bool isTree();
  bool isConnected();

  std::vector<bool> existID;
  std::vector<long long int> smallNodeMapedID; //Scene Node ID --> Graph Node ID
  std::vector<long long int> bigNodeMapedID; //Graph Node Id --> Scene Node ID

private:
  GraphType m_graphType;
  std::vector< std::vector<double> > m_matrixGraph;
  std::vector< std::vector<Edge> > m_listGraph;

  long long int N;
  bool flagNan, flagNeg;
  bool flagNanCap, flagNegCap;
  bool flagNanFlow, flagNegFlow;

  bool dfs(long long int v, int c); //Bipartite Check Function
  std::vector< int > color;

  bool isCycle(long long int c, long long int p); //Tree Check Function
  std::vector< int > prev;
};

#endif
