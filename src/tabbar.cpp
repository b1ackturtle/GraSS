#include "tabbar.h"
#include "tabwidget.h"

#include <qaction.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qevent.h>
#include <qmenu.h>
#include <qstyle.h>
#include <qurl.h>

#include <QDebug>

TabShortcut::TabShortcut(int tab, const QKeySequence &key, QWidget *parent)
  : QShortcut(key, parent), m_tab(tab)
{
}

int TabShortcut::tab()
{
  return m_tab;
}

TabBar::TabBar(QWidget *parent) : QTabBar(parent),
				  m_viewTabBarAction(0), m_showTabBarWhenOneTab(true)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  setAcceptDrops(true);
  setElideMode(Qt::ElideRight);
  setUsesScrollButtons(true);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
	  this, SLOT(contextMenuRequested(const QPoint &)));

  QString alt = QLatin1String("Ctrl+%1");
  for(int i = 0; i < 9; i++) {
    int key = i + 1;
    TabShortcut *tabShortCut = new TabShortcut(i, alt.arg(key), this);
    connect(tabShortCut, SIGNAL(activated()), this, SLOT(selectTabAction()));
  }

  m_viewTabBarAction = new QAction(this);
  updateViewToolBarAction();
  connect(m_viewTabBarAction, SIGNAL(triggered()),
	  this, SLOT(viewTabBar()));

  setMovable(true);
}

bool TabBar::showTabBarWhenOneTab() const
{
  return m_showTabBarWhenOneTab;
}

void TabBar::setShowTabBarWhenOneTab(bool enabled)
{
  m_showTabBarWhenOneTab = enabled;
  updateVisibility();
}

QAction *TabBar::viewTabBarAction() const
{
  return m_viewTabBarAction;
}

QTabBar::ButtonPosition TabBar::freeSide()
{
  QTabBar::ButtonPosition side = (QTabBar::ButtonPosition)style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, 0, this);
  side = (side == QTabBar::LeftSide) ? QTabBar::RightSide : QTabBar::LeftSide;
  return side;
}

void TabBar::updateViewToolBarAction()
{
  bool show = showTabBarWhenOneTab();
  if(count() > 1) show = true;
  m_viewTabBarAction->setText(!show ? tr("Show Tab Bar") : tr("Hide Tab Bar"));
}

void TabBar::viewTabBar()
{
  setShowTabBarWhenOneTab(!showTabBarWhenOneTab());
  updateViewToolBarAction();
}

void TabBar::selectTabAction()
{
  int index = qobject_cast<TabShortcut*>(sender())->tab();
  setCurrentIndex(index);
}

void TabBar::contextMenuRequested(const QPoint &position)
{
  QMenu menu;
  TabWidget *tabWidget = qobject_cast<TabWidget*>(parentWidget());
  if(!tabWidget) return;

  menu.addAction(tabWidget->newTabAction());
  int index = tabAt(position);
  if(index != -1) {
    QAction *action = menu.addAction(tr("Duplicate Tab"), this, SLOT(cloneTab()));
    action->setData(index);

    menu.addSeparator();

    action = menu.addAction(tr("&Rename Tab"),
			    this, SLOT(renameTab()));
    action->setData(index);

    menu.addSeparator();

    action = menu.addAction(tr("&Close Tab"),
			    this, SLOT(closeTab()), QKeySequence::Close);
    action->setData(index);

    action = menu.addAction(tr("Close &Other Tabs"),
			    this, SLOT(closeOtherTabs()));
    action->setData(index);
  }

  menu.exec(QCursor::pos());
}

void TabBar::cloneTab()
{
  if(QAction *action = qobject_cast<QAction*>(sender())) {
    int index = action->data().toInt();
    emit cloneTab(index);
  }
}

void TabBar::closeTab()
{
  if(QAction *action = qobject_cast<QAction*>(sender())) {
    int index = action->data().toInt();
    emit closeTab(index);
  }
}

void TabBar::closeOtherTabs()
{
  if(QAction *action = qobject_cast<QAction*>(sender())) {
    int index = action->data().toInt();
    emit closeOtherTabs(index);
  }
}

void TabBar::renameTab()
{
  if(QAction *action = qobject_cast<QAction*>(sender())) {
    int index = action->data().toInt();
    emit renameTab(index);
  }
}

void TabBar::mouseDoubleClickEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    if(tabAt(event->pos()) == -1)
      emit newTab();
    else
      emit renameTab(tabAt(event->pos()));

    return;
  }
  QTabBar::mouseDoubleClickEvent(event);
}

void TabBar::mouseReleaseEvent(QMouseEvent *event)
{
  if(event->button() == Qt::MidButton) {
    int index = tabAt(event->pos());
    if(index != -1) {
      emit closeTab(index);
      return;
    }
  }

  QTabBar::mouseReleaseEvent(event);
}

QSize TabBar::tabSizeHint(int index) const
{
  QSize sizeHint = QTabBar::tabSizeHint(index);
  QFontMetrics fm = fontMetrics();
  return sizeHint.boundedTo(QSize(fm.width(QLatin1Char('M')) * 18, sizeHint.height()));
}

void TabBar::tabInserted(int position)
{
  Q_UNUSED(position);
  updateVisibility();
}

void TabBar::tabRemoved(int position)
{
  Q_UNUSED(position);
  updateVisibility();
}

void TabBar::updateVisibility()
{
  setVisible((count()) > 1 || m_showTabBarWhenOneTab);
  bool enabled = (count() == 1);
  if(m_viewTabBarAction->isEnabled() != enabled)
    m_viewTabBarAction->setEnabled(enabled);
  updateViewToolBarAction();
}
