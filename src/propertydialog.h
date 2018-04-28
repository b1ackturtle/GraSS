#ifndef PROPERTYDIALOG_H
#define PROPERTYDIALOG_H

#include <QDialog>

class QLabel;
class QComboBox;
class QDialogButtonBox;
class QLineEdit;
class QVBoxLayout;
class QGraphicsItem;

class PropertyDialog : public QDialog
{
  Q_OBJECT

public:
  PropertyDialog(QList< QGraphicsItem* > items, QWidget *parent = 0);
  int dialogType();

  QColor color;

  QLineEdit *nameLineEdit;
  QLineEdit *valueLineEdit;
  QLineEdit *flowLineEdit;
  QLineEdit *capacityLineEdit;
  QLineEdit *widthLineEdit;
  QLineEdit *sizeLineEdit;
  QComboBox *colorComboBox;
  QComboBox *fontSizeComboBox;
  QComboBox *cornerComboBox;

private slots:
  void colorDialog(int index);

private:
  void setVisibleAndValue();
  void createWidgets();
  void createConnections();
  void createLayouts();

  QLabel *idLabel;
  QLineEdit *idLineEdit;
  
  QLabel *nameLabel;
  QLabel *valueLabel;
  QLabel *flowLabel;
  QLabel *capacityLabel;
  QLabel *fontSizeLabel;
  QLabel *colorLabel;
  QLabel *widthLabel;
  QLabel *sizeLabel;
  QLabel *cornerLabel;

  QDialogButtonBox *buttonBox;

  QList< QGraphicsItem* > m_items;
  std::vector< bool > itemType;
};

#endif
