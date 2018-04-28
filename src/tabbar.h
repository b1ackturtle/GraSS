#ifndef TABBAR_H
#define TABBAR_H

#include <qshortcut.h>
#include <qtabbar.h>

#include "tabwidget.h"

class TabBar : public QTabBar
{
  Q_OBJECT

signals:
  void newTab();
  void cloneTab(int index);
  void closeTab(int index);
  void closeOtherTabs(int index);
  void renameTab(int index);

public:
  TabBar(QWidget *parent = 0);

  bool showTabBarWhenOneTab() const;
  void setShowTabBarWhenOneTab(bool enabled);

  QAction *viewTabBarAction() const;
  QTabBar::ButtonPosition freeSide();

protected:
  void mouseDoubleClickEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  QSize tabSizeHint(int index) const;

  void tabInserted(int position);
  void tabRemoved(int position);

private slots:
  void selectTabAction();
  void cloneTab();
  void closeTab();
  void closeOtherTabs();
  void renameTab();
  void contextMenuRequested(const QPoint &position);
  void updateViewToolBarAction();
  void viewTabBar();

private:
  void updateVisibility();
  friend class TabWidget;

  QPoint m_dragStartPos;

  QAction *m_viewTabBarAction;
  bool m_showTabBarWhenOneTab;

};

class TabShortcut : public QShortcut
{
  Q_OBJECT

public:
  int tab();
  TabShortcut(int tab, const QKeySequence &key, QWidget *parent);

private:
  int m_tab;
};
  

#endif
