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

  void serviceDiscovered(const QBluetoothServiceInfo &serviceInfo);

  void on_buttonPair_clicked();
  void on_buttonUnpair_clicked();

  void finishPairing(QBluetoothAddress addr, QBluetoothLocalDevice::Pairing status);

private:
  Ui::BtConnector *ui;
  QBluetoothAddress localDevAddr;
  QBluetoothLocalDevice *localDevice;
  QBluetoothDeviceDiscoveryAgent *localDiscoveryAgent;
  QBluetoothServiceDiscoveryAgent *serviceDiscoveryAgent;


  QBluetoothDeviceInfo getSelectedRemoteDevice();

};

#endif // BTCONNECTOR_H
