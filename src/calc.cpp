#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "calc.h"
#include "graphicsview.h"
#include "graphdata.hpp"
#include "mainwindow.h"
#include "tools.hpp"
#include "tabwidget.h"

Calc::Calc(const GraphData &graph, QWidget *parent)
  : QDialog(parent)
  , tabTitle(tr(""))
{
  m_parent = parent;
  graphData = &graph;
}

GraphicsView* Calc::createTab(bool copyFlag)
{
  TabWidget *t = static_cast<MainWindow*>(m_parent)->tabs;
  if(copyFlag) // duplicate tab
    t->cloneTab(t->currentIndex());
  else        // new tab
    t->whiteNewTab();
    
  t->setTabText(t->currentIndex(), tabTitle);
  return static_cast<GraphicsView*>(t->currentWidget());
}

bool Calc::checkNan()
{
  if(!graphData->hasNan()) return false;

  return !Tools::question(m_parent, tr("Question"), tr("Graph is incomplete."),
			  tr("It has NaN values.\n"
			     "It may not show accurate answers.\n"
			     "Do you want to run the calculation?\n"),
			  tr("Run"), tr("Cancel"));
}

bool Calc::checkNeg()
{
  if(!graphData->hasNeg()) return false;

  return !Tools::question(m_parent,tr("Question"), tr("Graph is incomplete."),
			  tr("It has negative values.\n"
			     "It may not show accurate answers.\n"
			     "Do you want to run the calculation?\n"),
			  tr("Run"), tr("Cancel"));
}

bool Calc::checkNanCap()
{
  if(!graphData->hasNanCap()) return false;

  return !Tools::question(m_parent, tr("Question"), tr("Graph is incomplete."),
			  tr("It has NaN capacities.\n"
			     "It may not show accurate answers.\n"
			     "Do you want to run the calculation?\n"),
			  tr("Run"), tr("Cancel"));
}

bool Calc::checkNegCap()
{
  if(!graphData->hasNegCap()) return false;

  return !Tools::question(m_parent,tr("Question"), tr("Graph is incomplete."),
			  tr("It has negative capacities.\n"
			     "It may not show accurate answers.\n"
			     "Do you want to run the calculation?\n"),
			  tr("Run"), tr("Cancel"));
}

bool Calc::checkNanFlow()
{
  if(!graphData->hasNanFlow()) return false;

  return !Tools::question(m_parent, tr("Question"), tr("Graph is incomplete."),
			  tr("It has NaN flows.\n"
			     "It may not show accurate answers.\n"
			     "Do you want to run the calculation?\n"),
			  tr("Run"), tr("Cancel"));
}

bool Calc::checkNegFlow()
{
  if(!graphData->hasNegFlow()) return false;

  return !Tools::question(m_parent,tr("Question"), tr("Graph is incomplete."),
			  tr("It has negative flows.\n"
			     "It may not show accurate answers.\n"
			     "Do you want to run the calculation?\n"),
			  tr("Run"), tr("Cancel"));
}
