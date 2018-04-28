#include "tabwidget.h"
#include "tabbar.h"
#include "graphicsview.h"
#include "tools.hpp"
#include "mainwindow.h"
#include "nodeitem.hpp"
#include "edgeitem.hpp"
#include "loopitem.hpp"
#include "initgraphdialog.h"
#include "layoutdialog.h"
#include "randomlayout.h"
#include "springlayout.h"
#include "geneticalgorithmlayout.h"

#include <QDebug>
#include <QTabBar>
#include <QAction>
#include <QSvgGenerator>

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <vector>

const qint32 MagicNumber = 0x10101515;
const qint16 VersionNumber = 1;

TabWidget::TabWidget(QWidget *parent)
  : QTabWidget(parent)
  , m_newTabAction(0)
  , m_closeTabAction(0)
  , m_nextTabAction(0)
  , m_previousTabAction(0)
  , m_tabBar(new TabBar(this))
  , m_parent(parent)
{
  setTabBar(m_tabBar);
  setDocumentMode(true);
  setTabsClosable(true);

  connect(m_tabBar, SIGNAL(newTab()), this, SLOT(newTab()));
  connect(m_tabBar, SIGNAL(closeTab(int)), this, SLOT(closeTab(int)));
  connect(m_tabBar, SIGNAL(closeOtherTabs(int)), this, SLOT(closeOtherTabs(int)));
  connect(m_tabBar, SIGNAL(cloneTab(int)), this, SLOT(cloneTab(int)));
  connect(m_tabBar, SIGNAL(renameTab(int)), this, SLOT(renameTab(int)));

  connect(m_tabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));

  connect(this, SIGNAL(currentChanged(int)), this, SLOT(changeView(int)));

  m_newTabAction = new QAction(this);
  m_newTabAction->setShortcuts(QKeySequence::AddTab);
  m_newTabAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
  m_newTabAction->setIconVisibleInMenu(false);
  connect(m_newTabAction, SIGNAL(triggered()), this, SLOT(newTab()));

  m_closeTabAction = new QAction(this);
  m_closeTabAction->setShortcuts(QKeySequence::Close);
  connect(m_closeTabAction, SIGNAL(triggered()), this, SLOT(closeTab()));

  m_nextTabAction = new QAction(this);
  connect(m_nextTabAction, SIGNAL(triggered()), this, SLOT(nextTab()));

  m_previousTabAction = new QAction(this);
  connect(m_previousTabAction, SIGNAL(triggered()), this, SLOT(previousTab()));

  retranslate();
}

void TabWidget::changeView(int index)
{
  Q_UNUSED(index);
  view = static_cast<GraphicsView*>(currentWidget());
}

void TabWidget::modifiedTab(bool flag)
{
  int index = currentIndex();
  QString title = tabText(index);
  std::string str = title.toLocal8Bit().constData();

  if(flag && str.find("[*]") == std::string::npos)
    setTabText(index, title+"[*]");
  if(!flag)
    setTabText(index, title.replace("[*]", ""));

  m_parent->setWindowModified(flag);
}

void TabWidget::newTab()
{
  QString title = "Graph%1";
  QGraphicsScene *scene = new QGraphicsScene();
  scene->setSceneRect(0, 0, 1280, 720);
  GraphicsView *view = new GraphicsView(scene, this);
  addTab(view, title.arg(count()));
  setCurrentWidget(view);

  InitGraphDialog *igd = new InitGraphDialog(view, this);
  igd->setModal(true);
  #if QT_VERSION >= 0x050000
  igd->exec();
  #else
  igd->show();
  #endif

  connect(view, SIGNAL(modifiedScene(bool)), this, SLOT(modifiedTab(bool)));
  connect(view->undoStack(), SIGNAL(canUndoChanged(bool)), this, SLOT(modifiedTab(bool)));
}

void TabWidget::whiteNewTab()
{
  QString title = "Graph%1";
  QGraphicsScene *scene = new QGraphicsScene();
  scene->setSceneRect(0, 0, 1280, 720);
  GraphicsView *view = new GraphicsView(scene, this);
  addTab(view, title.arg(count()));
  setCurrentWidget(view);

  connect(view, SIGNAL(modifiedScene(bool)), this, SLOT(modifiedTab(bool)));
  connect(view->undoStack(), SIGNAL(canUndoChanged(bool)), this, SLOT(modifiedTab(bool)));
}

void TabWidget::closeTab(int index)
{
  if(index < 0) index = currentIndex();
  if(index < 0 || index >= count()) return;

  if(okToClearData(index)) {
    modifiedTab(false);
    if(count() == 1)
      emit lastTabClosed();
    else
      removeTab(index);
  }
}

void TabWidget::closeOtherTabs(int index)
{
  if(index == -1) return;
  for(int i = count() - 1; i > index; i--) closeTab(i);
  for(int i = index - 1; i >= 0; i--) closeTab(i);
}

void TabWidget::cloneTab(int index)
{
  if(index < 0) index = currentIndex();
  if(index < 0 || index >= count()) return;

  QString title = "Graph%1";
  QGraphicsScene *scene = new QGraphicsScene();
  scene->setSceneRect(0, 0, 1280, 720);
  GraphicsView *clone = static_cast<GraphicsView*>(currentWidget());
  GraphicsView *view = new GraphicsView(*clone, scene, this);
  addTab(view, title.arg(count()));
  setCurrentWidget(view);

  connect(view, SIGNAL(modifiedScene(bool)), this, SLOT(modifiedTab(bool)));
  connect(view->undoStack(), SIGNAL(canUndoChanged(bool)), this, SLOT(modifiedTab(bool)));
}

void TabWidget::renameTab(int index)
{
  if(index < 0) index = currentIndex();
  if(index < 0 || index >= count()) return;

  bool changeFlag = true;
  QString newName = QInputDialog::getText(this, tr("Change Name"),
					  tr("Insert New Tab Name"),
					  QLineEdit::Normal,
					  tabText(index).replace("[*]", ""),
					  &changeFlag);
  if(changeFlag) {
    if(isModified(index)) setTabText(index, newName + "[*]");
    else setTabText(index, newName);
  }
}

void TabWidget::nextTab()
{
  int next = currentIndex() + 1;
  if(next == count()) next = 0;
  setCurrentIndex(next);
}

void TabWidget::previousTab()
{
  int next = currentIndex() - 1;
  if(next == -1) next = count() - 1;
  setCurrentIndex(next);
}

QAction *TabWidget::newTabAction() const
{
  return m_newTabAction;
}

QAction *TabWidget::closeTabAction() const
{
  return m_closeTabAction;
}

QAction *TabWidget::nextTabAction() const
{
  return m_nextTabAction;
}

QAction *TabWidget::previousTabAction() const
{
  return m_previousTabAction;
}

bool TabWidget::isModified(int index)
{
  QString title = tabText(index);
  std::string str = title.toLocal8Bit().constData();

  return (str.find("[*]") != std::string::npos);
}

void TabWidget::fileOpen()
{
  if(!okToClearData(currentIndex())) return;
  const QString &filename = QFileDialog::getOpenFileName(this, tr("%1 - Open").arg(QApplication::applicationName()), ".", tr("GraSS (*.gss)"), 0);
  if(filename.isEmpty()) return;
  view->setWindowFilePath(filename);

  loadFile();
}

void TabWidget::loadFile()
{
  QFile *file= new QFile(view->windowFilePath());
  QDataStream *in = new QDataStream;
  if(!openGraSSFile(file, *in)) return;
  in->setVersion(QDataStream::Qt_4_5);
  view->undoStack()->clear();
  view->scene()->clear();
  view->multipleEdgeTableClear();
  view->multipleLoopTableClear();
  view->resetMatrix();
  readItems(*in);
  static_cast<MainWindow*>(m_parent)->statusBar()->showMessage(tr("Loaded %1").arg(view->windowFilePath()), Tools::StatusTimeout);
  modifiedTab(false);
}

bool TabWidget::fileSave()
{
  const QString filename = view->windowFilePath();
  if(filename.isEmpty() || filename == tr("Unnamed"))
    return fileSaveAs();
  QFile file(filename);
  if(!file.open(QIODevice::WriteOnly))
    return false;
  QDataStream out(&file);
  out << MagicNumber << VersionNumber;
  out.setVersion(QDataStream::Qt_4_5);
  writeItems(out, view->scene()->items());
  file.close();
  modifiedTab(false);
  return true;
}

bool TabWidget::fileSaveAs()
{
  QString filename = QFileDialog::getSaveFileName(this, tr("%1 - Save As").arg(QApplication::applicationName()), tabText(currentIndex()).replace("[*]", ""), tr("GraSS (*.gss)"));
  if(filename.isEmpty())
    return false;
  if(!filename.toLower().endsWith(".gss"))
    filename += ".gss";
  view->setWindowFilePath(filename);
  return fileSave();
}

void TabWidget::fileImport()
{
  if(!okToClearData(currentIndex())) return;
  const QString &filename = QFileDialog::getOpenFileName(this, tr("%1 - Import").arg(QApplication::applicationName()), ".", tr("All files (*.*);;Text files (*.txt);;CSV files (*.csv)"), 0);
  if(filename.isEmpty()) return;

  view->undoStack()->clear();
  view->scene()->clear();
  view->multipleEdgeTableClear();
  view->multipleLoopTableClear();
  view->resetMatrix();

  view->setWindowFilePath(filename);

  QFile *file = new QFile(view->windowFilePath());

  long long int N, E;
  std::vector< Edge > edges;
  if(!openImportFile(file, N, E, edges)) return;

  GraphData graph(N, edges);

  LayoutSelectDialog *lsd = new LayoutSelectDialog();
  if(lsd->exec()) {
    switch(lsd->selectComboBox->currentIndex())
      {
      case 0: {
	RandomLayout *rl = new RandomLayout(view, N, edges, this);
	rl->run();

	break;
      }
      case 1: {
	if(!graph.isConnected()) {
	  bool ret = Tools::question(m_parent, tr("Question"),
				     tr("Graph is not connected."),
				     tr("It is not a connected graph.\n"
					"It may not show accurate results.\n"
					"Do you want to run the layout?\n"),
				     tr("Run"), tr("Cancel"));
	  if(!ret) return;
	}

	RandomLayout *rl = new RandomLayout(view, N, edges, this);
	rl->run();
	SpringLayout *sl = new SpringLayout(view, this);
	sl->run(lsd->coulombParamLineEdit->text().toDouble(),
		lsd->bounceParamLineEdit->text().toDouble(),
		lsd->animationCheckBox->checkState());

	break;
      }
      case 2:
	GeneticAlgorithmLayout *gl =
	  new GeneticAlgorithmLayout(view, N, edges, this);
	gl->run(lsd->genParamLineEdit->text().toLongLong(),
		lsd->popParamLineEdit->text().toLongLong(),
		lsd->eliteRateLineEdit->text().toDouble(),
		lsd->crossRateLineEdit->text().toDouble(),
		lsd->mutateRateLineEdit->text().toDouble());
	
	break;
      }
  }
  modifiedTab(false);
}

void TabWidget::fileExport()
{
  QString suffixes = filenameFilter(tr("Bitmap image"),
				    QImageWriter::supportedImageFormats());
  suffixes += tr(";;Vector image (*.svg)");
  const QString filename =
    QFileDialog::getSaveFileName(this,
				 tr("%1 - Export").arg(QApplication::applicationName()),
				 ".", suffixes);
  if(filename.isEmpty()) return;
  if(filename.toLower().endsWith(".svg"))
    exportSvg(filename);
  else
    exportImage(filename);
}

void TabWidget::exportSvg(const QString &filename)
{
  QSvgGenerator svg;
  svg.setFileName(filename);
  svg.setSize(QSize(view->scene()->width(), view->scene()->height()));
  {
    QPainter painter(&svg);
    paintScene(&painter);
  }
  static_cast<MainWindow*>(m_parent)->statusBar()->showMessage(tr("Exported %1").arg(filename), Tools::StatusTimeout);
}

void TabWidget::exportImage(const QString &filename)
{
  QImage image(view->width(), view->height(), QImage::Format_ARGB32);
  {
    QPainter painter(&image);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    paintScene(&painter);
  }
  if(image.save(filename))
    static_cast<MainWindow*>(m_parent)->statusBar()->showMessage(tr("Exported %1").arg(filename), Tools::StatusTimeout);
  else
    Tools::warning(this, tr("Error"), tr("Failed to export: %1").arg(filename));
}

void TabWidget::paintScene(QPainter *painter)
{
  QRectF oldSceneRect = view->scene()->sceneRect();
  QPointF minPos = view->mapToScene(QPoint(0, 0));
  QPointF maxPos = view->mapToScene(QPoint(view->width(), view->height()));
  QPointF delta = maxPos - minPos;
  view->scene()->setSceneRect(minPos.x(), minPos.y(), delta.x(), delta.y());

  QList<QGraphicsItem*> items = view->scene()->selectedItems();
  view->scene()->clearSelection();

  view->scene()->render(painter);

  view->scene()->setSceneRect(oldSceneRect);

  Q_FOREACH(QGraphicsItem *item, items) {
    item->setSelected(true);
  }
}

bool TabWidget::okToClearData(int index)
{
  if(isModified(index)) {
    setCurrentIndex(index);
    return Tools::okToClearData(&TabWidget::fileSave, this,
			       tr("Unsaved changes"), tr("Save unsaved changes?"));
  }
  return true;
}

bool TabWidget::openGraSSFile(QFile *file, QDataStream &in)
{
  if(!file->open(QIODevice::ReadOnly)) {
    return false;
  }
  in.setDevice(file);
  qint32 magicNumber;
  in >> magicNumber;
  if(magicNumber != MagicNumber) {
    Tools::warning(this, tr("Error"), tr("Failed to open file: %1")
		   .arg(file->errorString()));
    return false;
  }
  qint16 versionNumber;
  in >> versionNumber;
  if(versionNumber > VersionNumber) {
    Tools::warning(this, tr("Error"),
		   tr("%1 is not a %2 file")
		   .arg(file->fileName()).arg(QApplication::applicationName()));
    return false;
  }
  return true;
}

bool TabWidget::openImportFile(QFile *file, long long int &N, long long int &E,
			       std::vector< Edge > &edges)
{
  if(!file->open(QIODevice::ReadOnly))
    return false;
  QTextStream in(file);

  long long int lineCount = 0;
  QString str;
  QList<QString> space, csv;
  enum FormatType {Space = 0, Csv = 1};
  FormatType format;

  str = in.readLine();
  space = str.split(" ");
  csv = str.split(",");
  if(space.size() == 2) {
    N = space[0].toLongLong();
    E = space[1].toLongLong();
    format = Space;
  }
  else if(csv.size() == 2) {
    N = csv[0].toLongLong();
    E = csv[1].toLongLong();
    format = Csv;
  }
  else {
    Tools::warning(this, tr("Error"),
		   tr("%1 is not a Graph Text file").arg(file->fileName()));
    return false;
  }

  QList<QString> input;
  long long int from, to;
  double value;
  while(!in.atEnd()) {
    str = in.readLine();

    if(format == Space)
      input = str.split(" ");
    else if(format == Csv)
      input = str.split(",");

    if(input.size() == 2) {
      from = input[0].toLongLong();
      to = input[1].toLongLong();
      value = std::numeric_limits<double>::quiet_NaN();
      edges.push_back(Edge(from, to, value));
    }
    else if(input.size() == 3) {
      from = input[0].toLongLong();
      to = input[1].toLongLong();
      value = input[2].toDouble();
      edges.push_back(Edge(from, to, value));
    }
    else {
      Tools::warning(this, tr("Error"),
		     tr("%1 is not a Graph Text file").arg(file->fileName()));
      return false;
    }
    lineCount++;
  }
  if(lineCount != E) {
    Tools::warning(this, tr("Error"),
		   tr("%1 is not a Graph Text file").arg(file->fileName()));
    return false;
  }

  return true;
}

QString TabWidget::filenameFilter(const QString &name, const QList<QByteArray> formats)
{
  QStringList fileFormatList;
  Q_FOREACH(const QByteArray &ba, formats) {
    fileFormatList += ba;
  }
  if(fileFormatList.isEmpty())
    fileFormatList << "*";
  else
    fileFormatList.sort();

  QString fileFormats = QString("%1 (*.").arg(name);
  fileFormats += fileFormatList.join(" *.") + ")";

  return fileFormats;
}

void TabWidget::readItems(QDataStream &in)
{
  qint64 maxID;
  qint32 itemType;
  bool graphType;
  std::vector<NodeItem*> nodeIDTable;
  std::unordered_map<qint64, EdgeItem*> edgeAddrTable;
  std::unordered_map<qint64, LoopItem*> loopAddrTable;
  QList<QGraphicsItem*> tmpItemList;

  in >> graphType;
  in >> maxID;
  nodeIDTable = std::vector<NodeItem*>(maxID, NULL);
  while(!in.atEnd()) {
    in >> itemType;
    switch(itemType) {
    case NodeItemType: {
      qint64 id;
      in >> id;
      NodeItem *nodeItem = new NodeItem(QPoint(0, 0), id);
      in >> *nodeItem;

      nodeIDTable[id] = nodeItem;
      tmpItemList.append(nodeItem);
      break;
    }
    case EdgeItemType: {
      qint64 addr;
      NodeItem *tmp1 = new NodeItem(QPoint(100, 100), 0);
      NodeItem *tmp2 = new NodeItem(QPoint(200, 200), 0);
      EdgeItem *edgeItem = new EdgeItem(view->screenPos(tmp1),
					view->screenPos(tmp2),
					tmp1, tmp2, UNDIRECTED);
      in >> addr;
      in >> *edgeItem;

      edgeAddrTable[addr] = edgeItem;
      tmpItemList.append(edgeItem);
      break;
    }
    case LoopItemType: {
      qint64 addr;
      QPointF tmpPoint(0, 0);
      NodeItem *tmp = new NodeItem(QPoint(0, 0), 0);
      LoopItem *loopItem = new LoopItem(view->screenPos(tmp), tmp, UNDIRECTED);

      in >> addr;
      in >> *loopItem;

      loopAddrTable[addr] = loopItem;
      tmpItemList.append(loopItem);
      break;
    }
    default: Q_ASSERT(false);
    }
  }

  view->multipleEdgeTableClear();
  Q_FOREACH(QGraphicsItem *item, tmpItemList) {
    int itemType = item->type();
    switch(itemType) {
      case NodeItemType: {
	NodeItem *n = static_cast<NodeItem*>(item);
	int i = 0;
	Q_FOREACH(EdgeItem *e, n->edgeList()) {
	  n->replaceEdgeList(i, edgeAddrTable[(qint64)e]);
	  i++;
	}
	i = 0;
	Q_FOREACH(LoopItem *l, n->loopList()) {
	  n->replaceLoopList(i, loopAddrTable[(qint64)l]);
	  i++;
	}
	view->scene()->addItem(n);
	break;
      }
      case EdgeItemType: {
	EdgeItem *e = static_cast<EdgeItem*>(item);
	int idU = e->nodeU()->id(), idV = e->nodeV()->id();
	delete e->nodeU(); delete e->nodeV();
	e->setNodeU(nodeIDTable[idU]); e->setNodeV(nodeIDTable[idV]);
	e->setU(view->screenPos(e->nodeU()));
	e->setV(view->screenPos(e->nodeV()));

	pll nodePair = GraphicsView::nodeItemPair(e->nodeU(), e->nodeV());
	view->multipleEdgeTableAppend(nodePair, e);
	e->setMultiTable(view->multipleEdgeTable());
	view->scene()->addItem(e);
	break;
      }
      case LoopItemType: {
	LoopItem *l = static_cast<LoopItem*>(item);
	int id = l->node()->id();
	delete l->node();
	l->setNode(nodeIDTable[id]);

	view->multipleLoopTableAppend(l->node()->id(), l);
	l->setMultiTable(view->multipleLoopTable());
	view->scene()->addItem(l);
	break;
      }
      default: Q_ASSERT(false);
    }
  }

  view->nodeId = maxID;
  view->setGraphType(graphType);
}

void TabWidget::writeItems(QDataStream &out, const QList<QGraphicsItem*> &items)
{
  out << (view->graphType() == UNDIRECTED);
  out << (qint64)view->nodeId;

  Q_FOREACH(QGraphicsItem *item, items) {
    qint32 type = static_cast<qint32>(item->type());
    out << type;

    switch(type) {
    case NodeItemType:
      out << *static_cast<NodeItem*>(item); break;
    case EdgeItemType:
      out << *static_cast<EdgeItem*>(item); break;
    case LoopItemType:
      out << *static_cast<LoopItem*>(item); break;
    default: Q_ASSERT(false);
    }

  }
}

void TabWidget::retranslate()
{
  m_nextTabAction->setText(tr("Show Next Tab"));
  QList<QKeySequence> shortcuts;
  shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BraceRight));
  shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_PageDown));
  shortcuts.append(tr("Ctrl-]"));
  shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Less));
  shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Tab));
  m_nextTabAction->setShortcuts(shortcuts);

  m_previousTabAction->setText(tr("Show Previous Tab"));
  shortcuts.clear();
  shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BraceLeft));
  shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_PageUp));
  shortcuts.append(tr("Ctrl-["));
  shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Greater));
  shortcuts.append(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Tab));
  m_previousTabAction->setShortcuts(shortcuts);

  m_newTabAction->setText(tr("New &Tab"));
  m_closeTabAction->setText(tr("&Close Tab"));

  m_tabBar->updateViewToolBarAction();
}
