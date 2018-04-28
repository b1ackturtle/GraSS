#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "tools.hpp"

namespace Tools {
  void warning(QWidget *parent, const QString &title, const QString &text,
	       const QString &detailedText)
  {
    QScopedPointer<QMessageBox> messageBox(new QMessageBox(parent));

    if(parent) messageBox->setWindowModality(Qt::WindowModal);
    messageBox->setWindowTitle(QString("%1 - %2").arg(QApplication::applicationName()).arg(title));
    messageBox->setText(text);
    if(!detailedText.isEmpty())
      messageBox->setInformativeText(detailedText);
    messageBox->setIcon(QMessageBox::Warning);
    messageBox->addButton(QMessageBox::Ok);
    messageBox->exec();
  }

  bool question(QWidget *parent, const QString &title,
		const QString &text, const QString &detailedText,
		const QString &yesText, const QString &noText)
  {
    QScopedPointer<QMessageBox> messageBox(new QMessageBox(parent));

    if(parent)
      messageBox->setWindowModality(Qt::WindowModal);
    messageBox->setWindowTitle(QString("%1 - %2")
			       .arg(QApplication::applicationName()).arg(title));
    messageBox->setText(text);
    if(!detailedText.isEmpty())
      messageBox->setInformativeText(detailedText);
    messageBox->setIcon(QMessageBox::Question);
    QAbstractButton *yesButton = messageBox->addButton(yesText, QMessageBox::AcceptRole);
    messageBox->addButton(noText, QMessageBox::RejectRole);
    messageBox->setDefaultButton(qobject_cast<QPushButton*>(yesButton));
    messageBox->exec();
    return messageBox->clickedButton() == yesButton;
  }
}
