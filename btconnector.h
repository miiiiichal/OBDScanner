#ifndef BTCONNECTOR_H
#define BTCONNECTOR_H

#include <QDialog>

namespace Ui {
  class BtConnector;
}

class BtConnector : public QDialog
{
  Q_OBJECT

public:
  explicit BtConnector(QWidget *parent = 0);
  ~BtConnector();

private:
  Ui::BtConnector *ui;
};

#endif // BTCONNECTOR_H
