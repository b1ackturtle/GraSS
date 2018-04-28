#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QToolButton;
class QButtonGroup;
class QLabel;
class QGraphicsItem;
class QGraphicsScene;
class QGraphicsView;
class QFile;
class GraphicsView;
class InitGraphDialog;
class GraphData;
class TabWidget;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);

  GraphicsView *view;
  TabWidget *tabs;
  QList<MainWindow*> mainWindows;

private slots:
  void newWindow();
  void undo();
  void redo();
  void selectAllNode();
  void selectInverse();
  void selectColorEdge(int c);
  void deleteSelectedItem();
  void zoomReset();
  void zoomIn();
  void zoomOut();
  void addNode();
  void addEdge();
  void deleteItem();
  void moveItem();
  void infoItem();
  void calcWarshallFloyd();
  void calcDijkstra();
  void calcBellmanFord();
  void calcPrim();
  void calcKruskal();
  void calcEdmondsKarp();
  void calcDinic();
  void calcGoldbergTarjan();
  void calcResidualGraph();
  void calcMinimumCut();
  void calcMinimumCostFlow();
  void calcGomoryHuTree();
  void calcHeldKarp();
  void calcTspGA();
  void calcEulerPath();
  void calcBipartiteMatching();
  void calcTreeHeight();
  void calcTreeDiameter();
  void calcTarjan();
  void calcInducedSubgraph();
  void calcContraction();
  void calcGirth();
  void randomLayout();
  void springLayout();
  void geneticAlgorithmLayout();
  void insertCompleteGraph();
  void insertCompleteBipartite();
  void insertCycleGraph();
  void insertStarGraph();
  void insertWheelGraph();
  void insertGearGraph();
  void insertGridGraph();
  void insertPathGraph();
  void aboutToShowEditMenu();
  void aboutToShowWindowMenu();
  void about();
  void changeView(int index);
  void showWindow();
  void lastTabClosed();
  void exit();

protected:
  void closeEvent(QCloseEvent *event);

private:
  void createCentralWidget();
  void createActions();
  void createConnects();
  void createMenus();
  void createToolBars();
  void createStatusBar();

  InitGraphDialog *igd;
  QDialog *calcDialog;
  GraphData *graph;

  QMenu *fileMenu;
  QMenu *editMenu;
  QMenu *calcMenu;
  QMenu *layoutMenu;
  QMenu *insertMenu;
  QMenu *viewMenu;
  QMenu *windowMenu;
  QMenu *helpMenu;

  QMenu *selectColorEdgeMenu;

  QMenu *shortestPathMenu;
  QMenu *minimumSpanningTreeMenu;
  QMenu *maximumFlowMenu;
  QMenu *hamiltonianPathMenu;

  QToolBar *fileToolBar;
  QToolBar *toolsToolBar;
  QButtonGroup *toolsButtonGroup;

  QToolButton *addNodeButton;
  QToolButton *addEdgeButton;
  QToolButton *deleteItemButton;
  QToolButton *moveItemButton;
  QToolButton *itemPropertyButton;

  QAction *newWindowAction;
  QAction *openAction;
  QAction *importAction;
  QAction *closeWindowAction;
  QAction *saveAction;
  QAction *saveAsAction;
  QAction *exportAction;
  QAction *exitAction;

  QAction *undoAction;
  QAction *redoAction;
  QAction *selectAllAction;
  QAction *selectInvAction;
  QAction *selectColorEdgeAction[5];
  QAction *deleteSelectAction;

  QAction *zoomResetAction;
  QAction *zoomInAction;
  QAction *zoomOutAction;
  
  QAction *addNodeAction;
  QAction *addEdgeAction;
  QAction *deleteItemAction;
  QAction *moveItemAction;
  QAction *itemPropertyAction;

  QAction *wfCalcAction;
  QAction *dijkCalcAction;
  QAction *bellmanCalcAction;
  QAction *primCalcAction;
  QAction *kruskalCalcAction;
  QAction *edmondsKarpCalcAction;
  QAction *dinicCalcAction;
  QAction *goldbergTarjanCalcAction;
  QAction *residualGraphCalcAction;
  QAction *minimumCutCalcAction;
  QAction *minimumCostFlowCalcAction;
  QAction *gomoryHuTreeCalcAction;
  QAction *heldKarpCalcAction;
  QAction *tspGACalcAction;
  QAction *eulerPathCalcAction;
  QAction *bipartiteMatchingCalcAction;
  QAction *treeHeightCalcAction;
  QAction *treeDiameterCalcAction;
  QAction *tarjanCalcAction;
  QAction *contractionCalcAction;
  QAction *inducedSubgraphCalcAction;
  QAction *girthCalcAction;

  QAction *randomLayoutAction;
  QAction *springLayoutAction;
  QAction *gaLayoutAction;

  QAction *completeGraphInsertAction;
  QAction *completeBipartiteInsertAction;
  QAction *cycleGraphInsertAction;
  QAction *starGraphInsertAction;
  QAction *wheelGraphInsertAction;
  QAction *gearGraphInsertAction;
  QAction *gridGraphInsertAction;
  QAction *pathGraphInsertAction;

  QAction *aboutAction;
  QAction *aboutQtAction;

  QLabel *locationLabel;
  QLabel *formulaLabel;
};

#endif
