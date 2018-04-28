#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <QGraphicsItem>
#include <QMessageBox>
#include <QApplication>
#include <unordered_map>

class EdgeItem;
class LoopItem;

const int NodeItemType = QGraphicsItem::UserType + 1;
const int EdgeItemType = QGraphicsItem::UserType + 2;
const int LoopItemType = QGraphicsItem::UserType + 3;

const QList<QColor> colorList = QList<QColor>() << Qt::black << Qt::red
							  << Qt::green << Qt::blue;
const QList< QString > colorStrList = QList<QString>() << "Black" << "Red" 
						       << "Green" << "Blue";

enum GraphType {UNDIRECTED, DIRECTED};

namespace std{
  struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2> &p) const {
      auto h1 = std::hash<T1>{}(p.first);
      auto h2 = std::hash<T2>{}(p.second);
      return h1 ^ h2;
    }
  };
}

typedef std::pair<long long int, long long int> pll;
typedef std::unordered_map<pll, QList<EdgeItem*>, std::pair_hash> MultiEdgeHash;
typedef std::unordered_map<long long int, QList<LoopItem*> > MultiLoopHash;

namespace Tools {
  const int StatusTimeout = 30000;

  void warning(QWidget *parent, const QString &title, const QString &text,
	       const QString &detailedText = QString());
  bool question(QWidget *parent, const QString &title, const QString &text,
		const QString &detailedText,
		const QString &yesText, const QString &noText);

  template<typename T>
  bool okToClearData(bool (T::*saveData)(), T*parent,
		     const QString &title, const QString &text,
		     const QString &detailedText = QString())
  {
    QScopedPointer<QMessageBox> messageBox(new QMessageBox(parent));
    messageBox->setWindowModality(Qt::WindowModal);
    messageBox->setIcon(QMessageBox::Question);
    messageBox->setWindowTitle(QString("%1 - %2").arg(QApplication::applicationName()).arg(title));
    messageBox->setText(text);
    if(!detailedText.isEmpty())
      messageBox->setInformativeText(detailedText);
    messageBox->addButton(QMessageBox::Save);
    messageBox->addButton(QMessageBox::Discard);
    messageBox->addButton(QMessageBox::Cancel);
    messageBox->setDefaultButton(QMessageBox::Save);
    messageBox->exec();
    if(messageBox->clickedButton() == messageBox->button(QMessageBox::Cancel))
      return false;
    if(messageBox->clickedButton() == messageBox->button(QMessageBox::Save))
      return (parent->*saveData)();
    return true;
  }
}

#endif
