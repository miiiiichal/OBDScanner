#ifndef BTCONNECTOR_H
#define BTCONNECTOR_H

#include <QDialog>
#include <QBluetoothLocalDevice>

#include <ui_btconnector.h>

namespace Ui {
  class BtConnector;
}

class BtConnector : public QDialog
{
  Q_OBJECT

public:
  explicit BtConnector(QBluetoothLocalDevice &localDev, QWidget *parent = 0);
  ~BtConnector();

private slots:


private:
  Ui::BtConnector *ui;
  QBluetoothLocalDevice *localDevice;
};

#endif // BTCONNECTOR_H
