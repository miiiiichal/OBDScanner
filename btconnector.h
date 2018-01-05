#ifndef BTCONNECTOR_H
#define BTCONNECTOR_H

#include <QDialog>
#include <QBluetoothLocalDevice>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothServiceDiscoveryAgent>

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

public slots:
  void doSth(QBluetoothServiceInfo &info);

private slots:
  void on_buttonScan_clicked();
  void startScanning();
  void stopScanning();

  void addFoundDevice(QBluetoothDeviceInfo);

  void on_buttonRemoteDeviceInfo_clicked();


  void on_buttonConnect_clicked();

  void serviceDiscovered(const QBluetoothServiceInfo &serviceInfo);

private:
  Ui::BtConnector *ui;
  QBluetoothAddress localDevAddr;
  QBluetoothDeviceDiscoveryAgent *localDiscoveryAgent;
  QBluetoothServiceDiscoveryAgent *serviceDiscoveryAgent;

};

#endif // BTCONNECTOR_H
