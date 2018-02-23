#ifndef BTCONNECTOR_H
#define BTCONNECTOR_H

#include <QDialog>
#include <QtDebug>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothServiceDiscoveryAgent>
#include "datakeeper.h"
#include "obddataexchanger.h"
#include <ui_btconnector.h>
#include <memory>


namespace Ui {
  class BtConnector;
}

class BtConnector : public QDialog
{
  Q_OBJECT
private:
  Ui::BtConnector *ui;
  QBluetoothAddress localDevAddr;
  //QBluetoothLocalDevice *localDevice;
  QBluetoothDeviceDiscoveryAgent *localDiscoveryAgent;
  QBluetoothServiceDiscoveryAgent *serviceDiscoveryAgent;
  //QBluetoothSocket *mySocket;
  QBluetoothDeviceInfo *selectedDevice;



  QBluetoothDeviceInfo getSelectedRemoteDevice();
  Logger *log;
  void debugInfo(QString);
  ObdDataExchanger *dataEx;

  std::shared_ptr<DataKeeper> spSharedData;


public:
 // explicit BtConnector(QBluetoothLocalDevice &, Logger *, QWidget *parent = 0);
 // explicit BtConnector(DataKeeper &a,  QWidget *parent = 0);
  explicit BtConnector(std::shared_ptr<DataKeeper> &a2,  QWidget *parent = 0);
  ~BtConnector();


signals:
  void conectedToSocket(QBluetoothSocket*);
  void conectedToSocket(QBluetoothSocket*, QString);
  void notConectedToSocket(QBluetoothDeviceInfo*);


public slots:


private slots:
  void on_buttonScan_clicked();
  void on_buttonPair_clicked();
  void on_buttonUnpair_clicked();
  void on_buttonRemoteDeviceInfo_clicked();
  void on_buttonConnect_clicked();
  void on_buttonDisconnect_clicked();



  void startScanning();
  void stopScanning();
  void addFoundDevice(QBluetoothDeviceInfo);

  void serviceDiscovered(const QBluetoothServiceInfo &serviceInfo);
  void finishPairing(QBluetoothAddress addr, QBluetoothLocalDevice::Pairing status);

  void socketConnected();
  void socketDisconnected();
  void socketRead();
  void socketError(QBluetoothSocket::SocketError);

};

#endif // BTCONNECTOR_H
