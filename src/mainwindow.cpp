#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "mainwindow.h"
#include "graphicsview.h"
#include "graphdata.hpp"
#include "initgraphdialog.h"
#include "warshallfloyd.h"
#include "dijkstra.h"
#include "bellmanford.h"
#include "prim.h"
#include "kruskal.h"
#include "edmondskarp.h"
#include "dinic.h"
#include "goldbergtarjan.h"
#include "heldkarp.h"
#include "tspga.h"
#include "residualgraph.h"
#include "minimumcut.h"
#include "gomoryhutree.h"
#include "minimumcostflow.h"
#include "bipartitematching.h"
#include "treeheight.h"
#include "treediameter.h"
#include "tarjan.h"
#include "inducedsubgraph.h"
#include "contraction.h"
#include "girth.h"
#include "eulerpath.h"
#include "layoutdialog.h"
#include "randomlayout.h"
#include "springlayout.h"
#include "geneticalgorithmlayout.h"
#include "completegraph.h"
#include "completebipartite.h"
#include "cyclegraph.h"
#include "stargraph.h"
#include "wheelgraph.h"
#include "geargraph.h"
#include "gridgraph.h"
#include "pathgraph.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"
#include "tabwidget.h"

#include <iostream>
#include <unordered_map>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  mainWindows.prepend(this);
  createCentralWidget();
  createActions();
  createMenus();
  createToolBars();

  //--- For Qt5 initgraphdialog->exec() ---
  #if QT_VERSION >= 0x050000
  setGeometry(100, 100, 1285, 810);
  show();
  tabs->newTab();
  view = static_cast<GraphicsView*>(tabs->currentWidget());
  #endif
  //-----------------------------------

  createConnects();
  createStatusBar();

  setWindowTitle(QApplication::applicationName());
}

void MainWindow::newWindow()
{
  MainWindow *window = new MainWindow();
  Q_FOREACH(MainWindow *w, mainWindows) {
    window->mainWindows.append(w);
    w->mainWindows.prepend(window);
  }

  window->resize(width(), height());
  window->show();
}

void MainWindow::undo()
{
  view->undoStack()->undo();
}

void MainWindow::redo()
{
  view->undoStack()->redo();
}

void MainWindow::selectAllNode()
{
  Q_FOREACH(QGraphicsItem *item, view->scene()->items()) {
    if(item->type() == NodeItemType)
      item->setSelected(true);
  }
}

void MainWindow::selectInverse()
{
  Q_FOREACH(QGraphicsItem *item, view->scene()->items()) {
    if(item->type() == NodeItemType)
      item->setSelected(!item->isSelected());
    else 
      item->setSelected(false);
  }
}

void MainWindow::selectColorEdge(int c)
{
  QColor color;
  if(c == 4) color = QColorDialog::getColor(Qt::yellow, this);
  else color = colorList[c];

  Q_FOREACH(QGraphicsItem *item, view->scene()->items()) {
    if(item->type() == NodeItemType) {
      item->setSelected(false);
    }
    else if(item->type() == EdgeItemType) {
      EdgeItem *edge = static_cast<EdgeItem*>(item);
      if(edge->color() == color) edge->setSelected(true);
      else edge->setSelected(false);
    }
    else if(item->type() == LoopItemType) {
      LoopItem *loop = static_cast<LoopItem*>(item);
      if(loop->color() == color) loop->setSelected(true);
      else loop->setSelected(false);
    }
  }
}

void MainWindow::deleteSelectedItem()
{
  QUndoCommand *deleteCommand = new DeleteCommand(view->scene()->selectedItems(), view);
  view->undoStack()->push(deleteCommand);

  view->scene()->update();
}

void MainWindow::zoomReset()
{
  view->resetMatrix();
}

void MainWindow::zoomIn()
{
  view->scale(1.1, 1.1);
}

void MainWindow::zoomOut()
{
  view->scale(0.9, 0.9);
}

void MainWindow::addNode()
{
  view->setToolFlag(view->tNode);
  view->setDragMode(QGraphicsView::NoDrag);
}

void MainWindow::addEdge()
{
  view->setToolFlag(view->tEdge);
  view->setDragMode(QGraphicsView::NoDrag);
}

void MainWindow::deleteItem()
{
  view->setToolFlag(view->tDelete);
  view->setDragMode(QGraphicsView::NoDrag);
}

void MainWindow::moveItem()
{
  view->setToolFlag(view->tMove);
  view->setDragMode(QGraphicsView::RubberBandDrag);
}

void MainWindow::infoItem()
{
  view->setToolFlag(view->tInfo);
  view->setDragMode(QGraphicsView::NoDrag);
}

void MainWindow::calcWarshallFloyd()
{
  graph = new GraphData(view);
  calcDialog = new WarshallFloyd(*graph, this);

  if(static_cast<WarshallFloyd*>(calcDialog)->checkNan()) return;
  if(static_cast<WarshallFloyd*>(calcDialog)->checkNeg()) return;

  calcDialog->show();
}

void MainWindow::calcDijkstra()
{
  graph = new GraphData(view);
  calcDialog = new Dijkstra(*graph, this);

  if(graph->hasNeg()) {
    Tools::warning(this, QString(), tr("Not Executable     "),
		   tr("This graph has negative edge costs.\n"
		      "Dijkstra Algorithm can not be applied to graphs"
		      "with negative edge costs."));
    return;
  }

  if(static_cast<Dijkstra*>(calcDialog)->checkNan()) return;

  calcDialog->show();
}

void MainWindow::calcBellmanFord()
{
  graph = new GraphData(view);
  calcDialog = new BellmanFord(*graph, this);
  
  if(static_cast<BellmanFord*>(calcDialog)->checkNan()) return;

  calcDialog->show();
}

void MainWindow::calcPrim()
{
  graph = new GraphData(view);
  calcDialog = new Prim(*graph, this);

  if(static_cast<Prim*>(calcDialog)->checkNan()) return;

  static_cast<Prim*>(calcDialog)->calc();
}

void MainWindow::calcKruskal()
{
  graph = new GraphData(view);
  calcDialog = new Kruskal(*graph, this);

  if(static_cast<Kruskal*>(calcDialog)->checkNan()) return;

  static_cast<Kruskal*>(calcDialog)->calc();
}

void MainWindow::calcEdmondsKarp()
{
  graph = new GraphData(view);
  calcDialog = new EdmondsKarp(*graph, this);

  if(static_cast<EdmondsKarp*>(calcDialog)->checkNanCap()) return;
  if(static_cast<EdmondsKarp*>(calcDialog)->checkNegCap()) return;

  calcDialog->show();
}

void MainWindow::calcDinic()
{
  graph = new GraphData(view);
  calcDialog = new Dinic(*graph, this);

  if(static_cast<Dinic*>(calcDialog)->checkNanCap()) return;
  if(static_cast<Dinic*>(calcDialog)->checkNegCap()) return;

  calcDialog->show();
}

void MainWindow::calcGoldbergTarjan()
{
  graph = new GraphData(view);
  calcDialog = new GoldbergTarjan(*graph, this);
  
  if(static_cast<GoldbergTarjan*>(calcDialog)->checkNanCap()) return;
  if(static_cast<GoldbergTarjan*>(calcDialog)->checkNegCap()) return;

  calcDialog->show();
}

void MainWindow::calcResidualGraph()
{
  graph = new GraphData(view);
  calcDialog = new ResidualGraph(*graph, this);

  if(static_cast<ResidualGraph*>(calcDialog)->checkNanCap()) return;
  if(static_cast<ResidualGraph*>(calcDialog)->checkNanFlow()) return;

  static_cast<ResidualGraph*>(calcDialog)->calc();
}

void MainWindow::calcMinimumCut()
{
  graph = new GraphData(view);
  calcDialog = new MinimumCut(*graph, this);

  calcDialog->show();
}

void MainWindow::calcGomoryHuTree()
{
  graph = new GraphData(view);
  calcDialog = new GomoryHuTree(*graph, this);

  static_cast<GomoryHuTree*>(calcDialog)->calc();
}

void MainWindow::calcMinimumCostFlow()
{
  graph = new GraphData(view);
  calcDialog = new MinimumCostFlow(*graph, this);

  calcDialog->show();
}

void MainWindow::calcHeldKarp()
{
  graph = new GraphData(view);
  calcDialog = new HeldKarp(*graph, this);

  if(graph->nodeSize() > static_cast<HeldKarp*>(calcDialog)->maxN()) {
    Tools::warning(this, QString(), tr("Not Executable     "),
		   tr("This graph has over 20 nodes."));
    return;
  }
  if(graph->hasNan()) {
    Tools::warning(this, QString(), tr("Not Executable     "),
		   tr("This graph has NaN value"));
    return;
  }

  calcDialog->show();
}

void MainWindow::calcTspGA()
{
  graph = new GraphData(view);
  calcDialog = new TspGA(*graph, this);

  calcDialog->show();
}

void MainWindow::calcEulerPath()
{
  graph = new GraphData(view);
  calcDialog = new EulerPath(*graph, this);

  calcDialog->show();
}

void MainWindow::calcBipartiteMatching()
{
  graph = new GraphData(view);

  if(!graph->isBipartite()) {
    Tools::warning(this, QString(), tr("Not Executable     "),
  		   tr("This is not bipartite graph.\n"));
    return;
  }

  calcDialog = new BipartiteMatching(*graph, this);
  
  static_cast<BipartiteMatching*>(calcDialog)->calc();
}

void MainWindow::calcTreeHeight()
{
  graph = new GraphData(view);

  if(!graph->isTree()) {
    Tools::warning(this, QString(), tr("Not Executable     "),
		   tr("This is not tree.\n"));
    return;
  }
  calcDialog = new TreeHeight(*graph, this);
  if(static_cast<TreeHeight*>(calcDialog)->checkNan()) return;

  static_cast<TreeHeight*>(calcDialog)->calc();
}

void MainWindow::calcTreeDiameter()
{
  graph = new GraphData(view);

  if(!graph->isTree()) {
    Tools::warning(this, QString(), tr("Not Executable     "),
		   tr("This is not tree.\n"));
    return;
  }

  calcDialog = new TreeDiameter(*graph, this);
  if(static_cast<TreeDiameter*>(calcDialog)->checkNan()) return;

  static_cast<TreeDiameter*>(calcDialog)->calc();
}

void MainWindow::calcTarjan()
{
  if(view->graphType() != DIRECTED) {
    Tools::warning(this, QString(), tr("Not Executable     "),
		   tr("This is not a directed graph.\n"
		      "Tarjan's SCC Algorithm can not be applied to undirected graphs."));
    return;
  }

  graph = new GraphData(view);
  calcDialog = new Tarjan(*graph, this);

  static_cast<Tarjan*>(calcDialog)->calc();
}

void MainWindow::calcInducedSubgraph()
{
  graph = new GraphData(view);
  calcDialog = new InducedSubgraph(*graph, this);
  
  calcDialog->show();
}

void MainWindow::calcContraction()
{
  graph = new GraphData(view);
  calcDialog = new Contraction(*graph, this);

  calcDialog->show();
}

void MainWindow::calcGirth()
{
  graph = new GraphData(view);
  calcDialog = new Girth(*graph, this);

  if(graph->hasNan()) {
    Tools::warning(this, QString(), tr("Not Executable       "),
		   tr("This graph has NaN edge costs.\n"
		      "Girth cannot be executed on a graph " 
		      "with NaN edge costs."));
    return;
  }
  
  static_cast<Girth*>(calcDialog)->calc();
}

void MainWindow::randomLayout()
{
  RandomLayout *rl = new RandomLayout(view, this);
  rl->run();
  view->undoStack()->push(rl);
  statusBar()->showMessage(tr("Random layout done"), Tools::StatusTimeout);
}

void MainWindow::springLayout()
{
  graph = new GraphData(view);
  if(!graph->isConnected()) {
    bool ret = Tools::question(this, tr("Question"),
  			       tr("Graph is not connected."),
  			       tr("It is not a connected graph.\n"
  				  "It may not show accurate results.\n"
  				  "Do you want to run the layout?\n"),
  			       tr("Run"), tr("Cancel"));
    if(!ret) return;
  }

  SpringLayout *sl = new SpringLayout(view, this);
  SpringLayoutDialog *sld = new SpringLayoutDialog(this);
  if(sld->exec()) {
    sl->run(sld->coulombParamLineEdit->text().toDouble(),
	    sld->bounceParamLineEdit->text().toDouble(),
	    sld->animationCheckBox->checkState());
    view->undoStack()->push(sl);
    statusBar()->showMessage(tr("Spring layout done"), Tools::StatusTimeout);
  }
}

void MainWindow::geneticAlgorithmLayout()
{
  GeneticAlgorithmLayout *gl = new GeneticAlgorithmLayout(view, this);
  GeneticAlgorithmLayoutDialog *gld = new GeneticAlgorithmLayoutDialog(this);
  if(gld->exec()) {
    gl->run(gld->genParamLineEdit->text().toLongLong(),
	    gld->popParamLineEdit->text().toLongLong(),
	    gld->eliteRateLineEdit->text().toDouble(),
	    gld->crossRateLineEdit->text().toDouble(),
	    gld->mutateRateLineEdit->text().toDouble());
    view->undoStack()->push(gl);
    statusBar()->showMessage(tr("Genetic algorithm layout done"), Tools::StatusTimeout);
  }
}

void MainWindow::insertCompleteGraph()
{
  CompleteGraph *cmpg = new CompleteGraph(view, this);
  CompleteGraphDialog *cgd = new CompleteGraphDialog(this);
  if(cgd->exec()) {
    cmpg->run(cgd->nodeCountLineEdit->text().toLongLong(),
	      cgd->radiusLineEdit->text().toLongLong());
    view->undoStack()->push(cmpg);
  }
}

void MainWindow::insertCompleteBipartite()
{
  CompleteBipartite *cb = new CompleteBipartite(view, this);
  CompleteBipartiteDialog *cbd = new CompleteBipartiteDialog(this);
  if(cbd->exec()) {
    cb->run(cbd->node1CountLineEdit->text().toLongLong(),
	    cbd->node2CountLineEdit->text().toLongLong(),
	    (bool)cbd->axisComboBox->currentIndex(),
	    cbd->spaceLineEdit->text().toLongLong(),
	    cbd->intervalLineEdit->text().toLongLong());
    view->undoStack()->push(cb);
  }
}

void MainWindow::insertCycleGraph()
{
  CycleGraph *cyg = new CycleGraph(view, this);
  CycleGraphDialog *cgd = new CycleGraphDialog(this);
  if(cgd->exec()) {
    cyg->run(cgd->nodeCountLineEdit->text().toLongLong(),
	     cgd->radiusLineEdit->text().toLongLong());
    view->undoStack()->push(cyg);
  }
}

void MainWindow::insertStarGraph()
{
  StarGraph *sg = new StarGraph(view, this);
  StarGraphDialog *sgd = new StarGraphDialog(this);
  if(sgd->exec()) {
    sg->run(sgd->nodeCountLineEdit->text().toLongLong(),
	    sgd->radiusLineEdit->text().toLongLong());
    view->undoStack()->push(sg);
  }
}

void MainWindow::insertWheelGraph()
{
  WheelGraph *wg = new WheelGraph(view, this);
  WheelGraphDialog *wgd = new WheelGraphDialog(this);
  if(wgd->exec()) {
    wg->run(wgd->wheelCountLineEdit->text().toLongLong(),
	    wgd->radiusLineEdit->text().toLongLong());
    view->undoStack()->push(wg);
  }
}

void MainWindow::insertGearGraph()
{
  GearGraph *gg = new GearGraph(view, this);
  GearGraphDialog *ggd = new GearGraphDialog(this);
  if(ggd->exec()) {
    gg->run(ggd->gearCountLineEdit->text().toLongLong(),
	    ggd->radiusLineEdit->text().toLongLong());
    view->undoStack()->push(gg);
  }
}

void MainWindow::insertGridGraph()
{
  GridGraph *gg = new GridGraph(view, this);
  GridGraphDialog *ggd = new GridGraphDialog(this);
  if(ggd->exec()) {
    gg->run(ggd->widthLineEdit->text().toLongLong(),
	    ggd->heightLineEdit->text().toLongLong(),
	    ggd->intervalLineEdit->text().toLongLong());
    view->undoStack()->push(gg);
  }
}

void MainWindow::insertPathGraph()
{
  PathGraph *pg = new PathGraph(view, this);
  PathGraphDialog *pgd = new PathGraphDialog(this);
  if(pgd->exec()) {
    pg->run(pgd->nodeCountLineEdit->text().toLongLong(),
	    pgd->intervalLineEdit->text().toLongLong(),
	    pgd->axisComboBox->currentIndex());
    view->undoStack()->push(pg);
  }
}

void MainWindow::aboutToShowEditMenu()
{
  editMenu->clear();
  editMenu->addAction(undoAction);
  editMenu->addAction(redoAction);
  editMenu->addSeparator();
  editMenu->addAction(selectAllAction);
  editMenu->addAction(selectInvAction);
  selectColorEdgeMenu = editMenu->addMenu(tr("Select Color Edge")); {
    for(int i = 0; i < 5; i++) {
      selectColorEdgeMenu->addAction(selectColorEdgeAction[i]);
    }
  }
  editMenu->addAction(deleteSelectAction);
  editMenu->addSeparator();
  editMenu->addAction(addNodeAction);
  editMenu->addAction(addEdgeAction);
  editMenu->addAction(deleteItemAction);
  editMenu->addAction(moveItemAction);
  editMenu->addAction(itemPropertyAction);
}

void MainWindow::aboutToShowWindowMenu()
{
  windowMenu->clear();
  windowMenu->addAction(tabs->nextTabAction());
  windowMenu->addAction(tabs->previousTabAction());
  windowMenu->addSeparator();
  for(int i = 0; i < mainWindows.count(); i++) {
    MainWindow *window = mainWindows.at(i);
    QAction *act = windowMenu->addAction(window->windowTitle(), this, SLOT(showWindow()));
    act->setData(i);
    act->setCheckable(true);
    if(window == this)
      act->setChecked(true);
  }
}

void MainWindow::about()
{
  QMessageBox::about(this, tr("About"),
		     tr("<h2>GraSS 1.0</h2>"
			"<p>Copyright &copy; 2017 Ichiho, OJIMA"
			"<p>This is a graph theory application. "
			"<p>This application can execute many algorithms and show their results."));
}

void MainWindow::changeView(int index)
{
  Q_UNUSED(index);
  disconnect(view->undoStack(), SIGNAL(canUndoChanged(bool)),
	     undoAction, SLOT(setEnabled(bool)));
  disconnect(view->undoStack(), SIGNAL(canRedoChanged(bool)),
	     redoAction, SLOT(setEnabled(bool)));

  view = static_cast<GraphicsView*>(tabs->currentWidget());
  QAbstractButton *checkedButton = toolsButtonGroup->checkedButton();
  view->setToolFlag((GraphicsView::ToolType)toolsButtonGroup->id(checkedButton));

  undoAction->setEnabled(view->undoStack()->canUndo());
  redoAction->setEnabled(view->undoStack()->canRedo());
  connect(view->undoStack(), SIGNAL(canUndoChanged(bool)),
  	  undoAction, SLOT(setEnabled(bool)));
  connect(view->undoStack(), SIGNAL(canRedoChanged(bool)),
  	  redoAction, SLOT(setEnabled(bool)));
}

void MainWindow::showWindow()
{
  if(QAction *action = static_cast<QAction*>(sender())) {
    QVariant v = action->data();
    if(v.canConvert<int>()) {
      int offset = qvariant_cast<int>(v);
      mainWindows.at(offset)->activateWindow();
      mainWindows.at(offset)->raise();
      mainWindows.at(offset)->tabs->currentWidget()->setFocus();
    }
  }
}

void MainWindow::lastTabClosed()
{
  if(tabs->isModified(0)) {
    int ret = QMessageBox::warning(this, QString(),
				   tr("Are you sure you want to close the tab?"
				      "This tab will not be saved."),
				   QMessageBox::Yes | QMessageBox::No,
				   QMessageBox::No);
    if(ret == QMessageBox::No)
      return;
  }

  close();
}

void MainWindow::exit()
{
  bool closeConfirm = false;
  Q_FOREACH(MainWindow* window, mainWindows) {
    for(int idx = 0; idx < window->tabs->count(); idx++) {
      if(window->tabs->isModified(idx)) {closeConfirm = true; break;}
    }
  }

  if(closeConfirm) {
    int ret = QMessageBox::warning(this, QString(),
				   tr("Are you sure you want to close all the windows?"
				      "  There are some tabs unsave."),
				   QMessageBox::Yes | QMessageBox::No,
				   QMessageBox::No);
    if(ret == QMessageBox::No)
      return;
  }

  static_cast<QApplication*>(parent())->quit();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  bool closeConfirm = false;
  for(int idx = 0; idx < tabs->count(); idx++) {
    if(tabs->isModified(idx)) {closeConfirm = true; break;}
  }

  if(closeConfirm) {
    int ret = QMessageBox::warning(this, QString(),
				   tr("Are you sure you want to close the window?"
				      "  There are some tabs unsave."),
				   QMessageBox::Yes | QMessageBox::No,
				   QMessageBox::No);
    if(ret == QMessageBox::No) {
      event->ignore();
      return;
    }
  }

  Q_FOREACH(MainWindow* window, mainWindows) {
    if(window == this) continue;
    for(int i = 0; i < window->mainWindows.count(); i++) {
      if(window->mainWindows.at(i) == this)
	window->mainWindows.removeAt(i);
    }
  }

  event->accept();
}

void MainWindow::createCentralWidget()
{
  tabs = new TabWidget(this);
  setCentralWidget(tabs);
  #if QT_VERSION >= 0x050000
  #else
  tabs->newTab();
  view = static_cast<GraphicsView*>(tabs->currentWidget());
  #endif
}

void MainWindow::createActions()
{
  newWindowAction = new QAction(tr("New &Window"), this);
  newWindowAction->setShortcut(QKeySequence::New);
  newWindowAction->setStatusTip(tr("Create a new window"));
  
  openAction = new QAction(tr("&Open..."), this);
  openAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
  openAction->setIconVisibleInMenu(false);
  openAction->setShortcut(QKeySequence::Open);
  openAction->setStatusTip(tr("Open an existing file"));

  importAction = new QAction(tr("&Import"), this);
  importAction->setStatusTip(tr("Import graph data"));

  closeWindowAction = new QAction(tr("Close Window"), this);
  closeWindowAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_W));
  closeWindowAction->setStatusTip(tr("Close the window"));

  saveAction = new QAction(tr("&Save"), this);
  saveAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton));
  saveAction->setIconVisibleInMenu(false);
  saveAction->setShortcut(QKeySequence::Save);
  saveAction->setStatusTip(tr("Save the file to disk"));

  saveAsAction = new QAction(tr("Save &As"), this);
  saveAsAction->setShortcut(QKeySequence::SaveAs);
  saveAsAction->setStatusTip(tr("Save the file under a new name"));

  exportAction = new QAction(tr("&Export"), this);
  exportAction->setStatusTip(tr("Export data to image"));

  exitAction = new QAction(tr("E&xit"), this);
  exitAction->setShortcut(QKeySequence::Quit);
  exitAction->setStatusTip(tr("Exit the application"));

  undoAction = new QAction(tr("&Undo"), this);
  undoAction->setShortcut(QKeySequence::Undo);
  undoAction->setEnabled(false);

  redoAction = new QAction(tr("&Redo"), this);
  redoAction->setShortcut(QKeySequence::Redo);
  redoAction->setEnabled(false);

  selectAllAction = new QAction(tr("Select &All Node"), this);
  selectAllAction->setShortcut(QKeySequence::SelectAll);

  selectInvAction = new QAction(tr("Select &Inverse Node"), this);
  selectInvAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));

  QPixmap px(15, 15);
  QIcon icon;
  for(int i = 0; i < 4; i++) {
    selectColorEdgeAction[i] = new QAction(colorStrList[i], this);
    px.fill(QColor(colorList[i]));
    icon.addPixmap(px);
    selectColorEdgeAction[i]->setIcon(icon);
  }
  selectColorEdgeAction[4] = new QAction(tr("others..."), this);

  deleteSelectAction = new QAction(tr("Delete Selected Item"), this);
  
  QList<QKeySequence> shortcuts;

  zoomResetAction = new QAction(tr("Zoom &Reset"), this);
  zoomResetAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));

  zoomInAction = new QAction(tr("Zoom &In"), this);
  shortcuts.clear();
  shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Plus));
  shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Equal));
  zoomInAction->setShortcuts(shortcuts);

  zoomOutAction = new QAction(tr("Zoom &Out"), this);
  shortcuts.clear();
  shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Minus));
  shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Underscore));
  zoomOutAction->setShortcuts(shortcuts);

  addNodeAction = new QAction(tr("Add Node"), this);
  addNodeAction->setShortcut(tr("N"));
  addNodeAction->setStatusTip(tr("Add vertexes"));

  addEdgeAction = new QAction(tr("Add Edge"), this);
  addEdgeAction->setShortcut(tr("E"));
  addEdgeAction->setStatusTip(tr("Add edges"));

  deleteItemAction = new QAction(tr("Delete Item"), this);
  deleteItemAction->setShortcut(tr("D"));
  deleteItemAction->setStatusTip(tr("Delete vertexes and edges"));

  moveItemAction = new QAction(tr("Move Item"), this);
  moveItemAction->setShortcut(tr("M"));
  moveItemAction->setStatusTip(tr("Move vertexes and edges"));

  itemPropertyAction = new QAction(tr("Item Properties"), this);
  itemPropertyAction->setShortcut(tr("I"));
  itemPropertyAction->setStatusTip(tr("Show the vertex or edge settings"));

  wfCalcAction = new QAction(tr("Warshall-Floyd"), this);
  wfCalcAction->setStatusTip(tr("Calculate minimum path by Warshall-Floyd Alogotihm"));

  dijkCalcAction = new QAction(tr("Dijkstra"), this);
  dijkCalcAction->setStatusTip(tr("Calculate minimum path by Dijkstra's Algorithm"));

  bellmanCalcAction = new QAction(tr("Bellman-Ford"), this);
  bellmanCalcAction->setStatusTip(tr("Calculate minimum path by Bellman-Ford Algorithm"));
  
  primCalcAction = new QAction(tr("Prim"), this);
  primCalcAction->setStatusTip(tr("Calculate minimum spanning tree by Prim's Algorithm"));

  kruskalCalcAction = new QAction(tr("Kruskal"), this);
  kruskalCalcAction->setStatusTip(tr("Calculate minimum spanning tree by Kruskal's Algorithm"));

  edmondsKarpCalcAction = new QAction(tr("Edmonds-Karp"), this);
  edmondsKarpCalcAction->setStatusTip(tr("Calculate maximum flow by Edmonds-Karp Algorithm"));

  dinicCalcAction = new QAction(tr("Dinic"), this);
  dinicCalcAction->setStatusTip(tr("Calculate maximum flow by Dinic Algotirhm"));

  goldbergTarjanCalcAction = new QAction(tr("Goldberg-Tarjan"), this);
  goldbergTarjanCalcAction->setStatusTip(tr("Calculate maximum flow by Goldberg-Tarjan Algorithm"));

  residualGraphCalcAction = new QAction(tr("Residual Graph"), this);
  residualGraphCalcAction->setStatusTip(tr("Display residual graph"));

  minimumCutCalcAction = new QAction(tr("Minimum Cut"), this);
  minimumCutCalcAction->setStatusTip(tr("Calculate minimum cut"));

  minimumCostFlowCalcAction = new QAction(tr("Minimum Cost Flow"), this);
  minimumCostFlowCalcAction->setStatusTip(tr("Calculate minimum cost flow"));

  gomoryHuTreeCalcAction = new QAction(tr("Gomory-Hu Tree"), this);
  gomoryHuTreeCalcAction->setStatusTip(tr("Calculate Gomory-Hu tree"));

  heldKarpCalcAction = new QAction(tr("Held-Karp"), this);
  heldKarpCalcAction->setStatusTip(tr("Calculate hamiltonian path by Held-Karp Algorithm"));

  tspGACalcAction = new QAction(tr("Genetic Algorithm"), this);
  tspGACalcAction->setStatusTip(tr("Calculate hamiltonian path by Genetic Algorithm"));

  eulerPathCalcAction = new QAction(tr("Euler Path"), this);
  eulerPathCalcAction->setStatusTip(tr("Calculate Euler path"));

  bipartiteMatchingCalcAction = new QAction(tr("Bipartite Matching"), this);
  bipartiteMatchingCalcAction->setStatusTip(tr("Calculate bipartite matching"));

  treeHeightCalcAction = new QAction(tr("Tree Height"), this);
  treeHeightCalcAction->setStatusTip(tr("Calculate tree height"));

  treeDiameterCalcAction = new QAction(tr("Tree Diameter"), this);
  treeDiameterCalcAction->setStatusTip(tr("Calculate tree Diameter"));

  tarjanCalcAction = new QAction(tr("Strongly Connected Components"), this);
  tarjanCalcAction->setStatusTip(tr("Calculate strongly connected components by Tarjan's Algorithm"));

  inducedSubgraphCalcAction = new QAction(tr("Induced Subgraph"), this);
  inducedSubgraphCalcAction->setStatusTip(tr("Calculate induced subgraph"));

  contractionCalcAction = new QAction(tr("Edge Contraction"), this);
  contractionCalcAction->setStatusTip(tr("Calculate edge contraction"));

  girthCalcAction = new QAction(tr("Girth"), this);
  girthCalcAction->setStatusTip(tr("Calculate girth"));

  randomLayoutAction = new QAction(tr("Random Layout"), this);
  randomLayoutAction->setStatusTip(tr("Set graph position with random"));

  springLayoutAction = new QAction(tr("Spring Layout"), this);
  springLayoutAction->setStatusTip(tr("Set graph position with spring layout"));

  gaLayoutAction = new QAction(tr("Genetic Algorithm Layout"), this);
  gaLayoutAction->setStatusTip(tr("Set graph position with genetic algorithm layout"));

  completeGraphInsertAction = new QAction(tr("Complete Graph"), this);
  completeGraphInsertAction->setStatusTip(tr("Insert K-complete graph"));

  completeBipartiteInsertAction = new QAction(tr("Complete Bipartite Graph"), this);
  completeBipartiteInsertAction->setStatusTip(tr("Insert complete bipartite graph"));

  cycleGraphInsertAction = new QAction(tr("Cycle Graph"), this);
  cycleGraphInsertAction->setStatusTip(tr("Insert cycle graph"));
  
  starGraphInsertAction = new QAction(tr("Star Graph"));
  starGraphInsertAction->setStatusTip(tr("Insert star graph"));
  
  wheelGraphInsertAction = new QAction(tr("Wheel Graph"));
  wheelGraphInsertAction->setStatusTip(tr("Insert wheel graph"));

  gearGraphInsertAction = new QAction(tr("Gear Graph"));
  gearGraphInsertAction->setStatusTip(tr("Insert gear graph"));

  gridGraphInsertAction = new QAction(tr("Grid Graph"));
  gridGraphInsertAction->setStatusTip(tr("Insert grid graph"));

  pathGraphInsertAction = new QAction(tr("Path Graph"));
  pathGraphInsertAction->setStatusTip(tr("Insert path graph"));

  aboutAction = new QAction(tr("&About"), this);
  aboutAction->setStatusTip(tr("Show the application's About box"));

  aboutQtAction = new QAction(tr("About &Qt"), this);
  aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
}

void MainWindow::createConnects()
{
  connect(newWindowAction, SIGNAL(triggered()), this, SLOT(newWindow()));
  connect(openAction, SIGNAL(triggered()), tabs, SLOT(fileOpen()));
  connect(importAction, SIGNAL(triggered()), tabs, SLOT(fileImport()));
  connect(closeWindowAction, SIGNAL(triggered()), this, SLOT(close()));
  connect(saveAction, SIGNAL(triggered()), tabs, SLOT(fileSave()));
  connect(saveAsAction, SIGNAL(triggered()), tabs, SLOT(fileSaveAs()));
  connect(exportAction, SIGNAL(triggered()), tabs, SLOT(fileExport()));
  connect(exitAction, SIGNAL(triggered()), this, SLOT(exit()));

  connect(undoAction, SIGNAL(triggered()), this, SLOT(undo()));
  connect(redoAction, SIGNAL(triggered()), this, SLOT(redo()));
  connect(view->undoStack(), SIGNAL(canUndoChanged(bool)),
  	  undoAction, SLOT(setEnabled(bool)));
  connect(view->undoStack(), SIGNAL(canRedoChanged(bool)),
  	  redoAction, SLOT(setEnabled(bool)));
  connect(selectAllAction, SIGNAL(triggered()), this, SLOT(selectAllNode()));
  connect(selectInvAction, SIGNAL(triggered()), this, SLOT(selectInverse()));

  QSignalMapper* signalMapper = new QSignalMapper(this);
  for(int i = 0; i < 5; i++) {
    connect(selectColorEdgeAction[i], SIGNAL(triggered()),
	    signalMapper, SLOT(map()));
    signalMapper->setMapping(selectColorEdgeAction[i], i);
  }
  connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(selectColorEdge(int)));

  connect(deleteSelectAction, SIGNAL(triggered()), this, SLOT(deleteSelectedItem()));

  connect(zoomResetAction, SIGNAL(triggered()), this, SLOT(zoomReset()));
  connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));
  connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));

  connect(addNodeAction, SIGNAL(triggered()), this, SLOT(addNode()));
  connect(addEdgeAction, SIGNAL(triggered()), this, SLOT(addEdge()));
  connect(deleteItemAction, SIGNAL(triggered()), this, SLOT(deleteItem()));
  connect(moveItemAction, SIGNAL(triggered()), this, SLOT(moveItem()));
  connect(itemPropertyAction, SIGNAL(triggered()), this, SLOT(infoItem()));

  connect(addNodeButton, SIGNAL(clicked()), this, SLOT(addNode()));
  connect(addEdgeButton, SIGNAL(clicked()), this, SLOT(addEdge()));
  connect(deleteItemButton, SIGNAL(clicked()), this, SLOT(deleteItem()));
  connect(moveItemButton, SIGNAL(clicked()), this, SLOT(moveItem()));
  connect(itemPropertyButton, SIGNAL(clicked()), this, SLOT(infoItem()));

  connect(addNodeAction, SIGNAL(triggered()), addNodeButton, SLOT(click()));
  connect(addEdgeAction, SIGNAL(triggered()), addEdgeButton, SLOT(click()));
  connect(deleteItemAction, SIGNAL(triggered()), deleteItemButton, SLOT(click()));
  connect(moveItemAction, SIGNAL(triggered()), moveItemButton, SLOT(click()));
  connect(itemPropertyAction, SIGNAL(triggered()), itemPropertyButton, SLOT(click()));
  
  connect(wfCalcAction, SIGNAL(triggered()), this, SLOT(calcWarshallFloyd()));
  connect(dijkCalcAction, SIGNAL(triggered()), this, SLOT(calcDijkstra()));
  connect(bellmanCalcAction, SIGNAL(triggered()), this, SLOT(calcBellmanFord()));
  connect(primCalcAction, SIGNAL(triggered()), this, SLOT(calcPrim()));
  connect(kruskalCalcAction, SIGNAL(triggered()), this, SLOT(calcKruskal()));
  connect(edmondsKarpCalcAction, SIGNAL(triggered()), this, SLOT(calcEdmondsKarp()));
  connect(dinicCalcAction, SIGNAL(triggered()), this, SLOT(calcDinic()));
  connect(goldbergTarjanCalcAction, SIGNAL(triggered()),
	  this, SLOT(calcGoldbergTarjan()));
  connect(residualGraphCalcAction, SIGNAL(triggered()), this, SLOT(calcResidualGraph()));
  connect(minimumCutCalcAction, SIGNAL(triggered()), this, SLOT(calcMinimumCut()));
  connect(minimumCostFlowCalcAction, SIGNAL(triggered()),
	  this, SLOT(calcMinimumCostFlow()));
  connect(gomoryHuTreeCalcAction, SIGNAL(triggered()), this, SLOT(calcGomoryHuTree()));
  connect(heldKarpCalcAction, SIGNAL(triggered()), this, SLOT(calcHeldKarp()));
  connect(tspGACalcAction, SIGNAL(triggered()), this, SLOT(calcTspGA()));
  connect(eulerPathCalcAction, SIGNAL(triggered()), this, SLOT(calcEulerPath()));
  connect(bipartiteMatchingCalcAction, SIGNAL(triggered()),
	  this, SLOT(calcBipartiteMatching()));
  connect(treeHeightCalcAction, SIGNAL(triggered()),
	  this, SLOT(calcTreeHeight()));
  connect(treeDiameterCalcAction, SIGNAL(triggered()),
	  this, SLOT(calcTreeDiameter()));
  connect(tarjanCalcAction, SIGNAL(triggered()), this, SLOT(calcTarjan()));
  connect(inducedSubgraphCalcAction, SIGNAL(triggered()),
	  this, SLOT(calcInducedSubgraph()));
  connect(contractionCalcAction, SIGNAL(triggered()), this, SLOT(calcContraction()));
  connect(girthCalcAction, SIGNAL(triggered()), this, SLOT(calcGirth()));

  connect(randomLayoutAction, SIGNAL(triggered()), this, SLOT(randomLayout()));
  connect(springLayoutAction, SIGNAL(triggered()), this, SLOT(springLayout()));
  connect(gaLayoutAction, SIGNAL(triggered()), this, SLOT(geneticAlgorithmLayout()));

  connect(completeGraphInsertAction, SIGNAL(triggered()),
	  this, SLOT(insertCompleteGraph()));
  connect(completeBipartiteInsertAction, SIGNAL(triggered()),
	  this, SLOT(insertCompleteBipartite()));
  connect(cycleGraphInsertAction, SIGNAL(triggered()), this, SLOT(insertCycleGraph()));
  connect(starGraphInsertAction, SIGNAL(triggered()), this, SLOT(insertStarGraph()));
  connect(wheelGraphInsertAction, SIGNAL(triggered()), this, SLOT(insertWheelGraph()));
  connect(gearGraphInsertAction, SIGNAL(triggered()), this, SLOT(insertGearGraph()));
  connect(gridGraphInsertAction, SIGNAL(triggered()), this, SLOT(insertGridGraph()));
  connect(pathGraphInsertAction, SIGNAL(triggered()), this, SLOT(insertPathGraph()));

  connect(editMenu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowEditMenu()));
  connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowWindowMenu()));

  connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
  connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

  connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(changeView(int)));
  connect(tabs, SIGNAL(lastTabClosed()), this, SLOT(lastTabClosed()));
}

void MainWindow::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(newWindowAction);
  fileMenu->addAction(tabs->newTabAction());
  fileMenu->addAction(openAction);
  fileMenu->addAction(importAction);
  fileMenu->addSeparator();
  fileMenu->addAction(closeWindowAction);
  fileMenu->addAction(tabs->closeTabAction());
  fileMenu->addSeparator();
  fileMenu->addAction(saveAction);
  fileMenu->addAction(saveAsAction);
  fileMenu->addAction(exportAction);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAction);

  editMenu = menuBar()->addMenu(tr("&Edit"));
  aboutToShowEditMenu();

  calcMenu = menuBar()->addMenu(tr("&Calc"));
  shortestPathMenu = calcMenu->addMenu(tr("Shortest Path")); {
    shortestPathMenu->addAction(wfCalcAction);
    shortestPathMenu->addAction(dijkCalcAction);
    shortestPathMenu->addAction(bellmanCalcAction); }
  minimumSpanningTreeMenu = calcMenu->addMenu(tr("Minimum Spanning Tree")); {
    minimumSpanningTreeMenu->addAction(primCalcAction);
    minimumSpanningTreeMenu->addAction(kruskalCalcAction);}
  maximumFlowMenu = calcMenu->addMenu(tr("Maximum Flow")); {
    maximumFlowMenu->addAction(edmondsKarpCalcAction);
    maximumFlowMenu->addAction(dinicCalcAction);
    maximumFlowMenu->addAction(goldbergTarjanCalcAction);}
  calcMenu->addAction(residualGraphCalcAction);
  calcMenu->addAction(minimumCutCalcAction);
  calcMenu->addAction(gomoryHuTreeCalcAction);
  calcMenu->addAction(minimumCostFlowCalcAction);
  calcMenu->addAction(bipartiteMatchingCalcAction);
  hamiltonianPathMenu = calcMenu->addMenu(tr("Hamiltonian Path")); {
    hamiltonianPathMenu->addAction(heldKarpCalcAction);
    hamiltonianPathMenu->addAction(tspGACalcAction);
  }
  calcMenu->addAction(eulerPathCalcAction);
  calcMenu->addAction(treeHeightCalcAction);
  calcMenu->addAction(treeDiameterCalcAction);
  calcMenu->addAction(inducedSubgraphCalcAction);
  calcMenu->addAction(contractionCalcAction);
  calcMenu->addAction(girthCalcAction);
  calcMenu->addAction(tarjanCalcAction);

  layoutMenu = menuBar()->addMenu(tr("&Layout"));
  layoutMenu->addAction(randomLayoutAction);
  layoutMenu->addAction(springLayoutAction);
  layoutMenu->addAction(gaLayoutAction);

  insertMenu = menuBar()->addMenu(tr("&Insert"));
  insertMenu->addAction(completeGraphInsertAction);
  insertMenu->addAction(completeBipartiteInsertAction);
  insertMenu->addAction(cycleGraphInsertAction);
  insertMenu->addAction(starGraphInsertAction);
  insertMenu->addAction(wheelGraphInsertAction);
  insertMenu->addAction(gearGraphInsertAction);
  insertMenu->addAction(gridGraphInsertAction);
  insertMenu->addAction(pathGraphInsertAction);

  viewMenu = menuBar()->addMenu(tr("&View"));
  viewMenu->addAction(zoomResetAction);
  viewMenu->addAction(zoomInAction);
  viewMenu->addAction(zoomOutAction);

  windowMenu = menuBar()->addMenu(tr("&Window"));
  aboutToShowWindowMenu();

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAction);
  helpMenu->addAction(aboutQtAction);
}

void MainWindow::createToolBars()
{
  fileToolBar = addToolBar(tr("&File"));
  fileToolBar->addAction(tabs->newTabAction());
  fileToolBar->addAction(openAction);
  fileToolBar->addAction(saveAction);

  toolsToolBar = addToolBar(tr("&Tools"));

  toolsButtonGroup = new QButtonGroup(this);

  addNodeButton = new QToolButton();
  addNodeButton->setToolTip(tr("Add Node"));
  addNodeButton->setCheckable(true);
  addNodeButton->setChecked(true);
  addNodeButton->setIcon(QIcon("://image/node.png"));

  addEdgeButton = new QToolButton();
  addEdgeButton->setToolTip(tr("Add Edge"));
  addEdgeButton->setCheckable(true);
  addEdgeButton->setIcon(QIcon("://image/edge.png"));

  deleteItemButton = new QToolButton();
  deleteItemButton->setToolTip(tr("Delete Item"));
  deleteItemButton->setCheckable(true);
  deleteItemButton->setIcon(QIcon("://image/delete.png"));

  moveItemButton = new QToolButton();
  moveItemButton->setToolTip(tr("Move Item"));
  moveItemButton->setCheckable(true);
  moveItemButton->setIcon(QIcon("://image/move.png"));

  itemPropertyButton = new QToolButton();
  itemPropertyButton->setToolTip(tr("Item Properties"));
  itemPropertyButton->setCheckable(true);
  itemPropertyButton->setIcon(QIcon("://image/info.png"));

  toolsButtonGroup->addButton(addNodeButton, 0);
  toolsButtonGroup->addButton(addEdgeButton, 1);
  toolsButtonGroup->addButton(deleteItemButton, 2);
  toolsButtonGroup->addButton(moveItemButton, 3);
  toolsButtonGroup->addButton(itemPropertyButton, 4);

  toolsToolBar->addWidget(addNodeButton);
  toolsToolBar->addWidget(addEdgeButton);
  toolsToolBar->addWidget(deleteItemButton);
  toolsToolBar->addWidget(moveItemButton);
  toolsToolBar->addWidget(itemPropertyButton);
}

void MainWindow::createStatusBar()
{
  statusBar()->showMessage("Welcome to GraSS");
}
