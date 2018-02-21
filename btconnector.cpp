#include "btconnector.h"
#include "logger.h"
#include "ui_btconnector.h"
#include <iostream>
#include <string>
#include <sstream>

BtConnector::BtConnector(std::shared_ptr<DataKeeper> &a2, QWidget *parent) :
    QDialog(parent),
    spSharedData(a2),
    ui(new Ui::BtConnector)
{
    ui->setupUi(this);
   // spSharedData->test="btConnector text by sharedptr";
    localDevAddr = spSharedData->localDevice->address();
    localDiscoveryAgent  = new QBluetoothDeviceDiscoveryAgent();
    serviceDiscoveryAgent = new QBluetoothServiceDiscoveryAgent(localDevAddr);

    //interface init vals
    ui->lineEdit_address->setText(localDevAddr.toString());
    ui->lineEdit_name->setText(spSharedData->localDevice->name());
    setWindowTitle("Bluetooth device management dialog");

    // my Signal->Slot actions
    connect(ui->buttonClose, SIGNAL(clicked()),this,SLOT(close()) );
    connect(localDiscoveryAgent , SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)) , this , SLOT(addFoundDevice(QBluetoothDeviceInfo)));
    connect(localDiscoveryAgent , SIGNAL(finished()) , this , SLOT(stopScanning()));
    connect(serviceDiscoveryAgent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)), this, SLOT(serviceDiscovered(QBluetoothServiceInfo)));
    connect(spSharedData->localDevice, SIGNAL(pairingDisplayPinCode(QBluetoothAddress,QString)), this, SLOT(displayPin(QBluetoothAddress,QString)));
    connect(spSharedData->localDevice,SIGNAL(pairingDisplayConfirmation(QBluetoothAddress,QString)),spSharedData->localDevice,SLOT(pairingConfirmation(bool)) );
    connect(spSharedData->localDevice,SIGNAL(pairingFinished(QBluetoothAddress,QBluetoothLocalDevice::Pairing)),this,SLOT(finishPairing(QBluetoothAddress,QBluetoothLocalDevice::Pairing)) );
    spSharedData->log->logDebbug("btConnector created");
}

BtConnector::~BtConnector()
{
  delete localDiscoveryAgent;
  delete serviceDiscoveryAgent;
  delete ui;
  spSharedData->log->logDebbug("btConnector destroyed");
}


void BtConnector::on_buttonScan_clicked()
{
    startScanning();
}

void BtConnector::startScanning(){

  localDiscoveryAgent->start();
  ui->buttonScan->setEnabled(false);
  spSharedData->log->logDebbug("Scanning BT devices statrs");
  }

void BtConnector::stopScanning(){

  localDiscoveryAgent->stop();
  ui->buttonScan->setEnabled(true);
  spSharedData->log->logDebbug("Scanning BT devices finished");
}
void BtConnector::addFoundDevice(QBluetoothDeviceInfo deviceInfo){
  spSharedData->log->logDebbug("Found device: "+deviceInfo.name() +" : "+deviceInfo.address().toString());
  if(deviceInfo.isValid() && !deviceInfo.isCached()){
    QListWidgetItem *foundDeviceName = new QListWidgetItem(deviceInfo.name() + " | " +deviceInfo.address().toString());
    if(spSharedData->localDevice->pairingStatus(deviceInfo.address())==1)
      foundDeviceName->setBackgroundColor(QColor("green"));
    QList<QListWidgetItem *> items = ui->listDevicesNearby->findItems(deviceInfo.name() + " | " +deviceInfo.address().toString(), Qt::MatchExactly );
    if(items.empty())
      ui->listDevicesNearby->addItem(foundDeviceName);
  }
}

void BtConnector::on_buttonRemoteDeviceInfo_clicked()
{
  QBluetoothDeviceInfo selectedRemoteDevice =getSelectedRemoteDevice();
  if(selectedRemoteDevice.isValid()){
      QBluetoothAddress remoteAddr = selectedRemoteDevice.address();
      QString str(selectedRemoteDevice.address().toString()+" | "+selectedRemoteDevice.name());
       ui->textEdit->setText(str);
      serviceDiscoveryAgent->setRemoteAddress(remoteAddr);
      serviceDiscoveryAgent->start();
  }
}

void BtConnector::serviceDiscovered(const QBluetoothServiceInfo &serviceInfo){
 //  ui->textEdit->setText("remoteServiceInfo");
  QString remoteDeviceInfo;

  remoteDeviceInfo.clear();
  //remoteDeviceInfo+=serviceInfo.QBluetoothUuid(QBluetoothUuid::SerialPort));
//Info.device().name();

  remoteDeviceInfo+="\nserviceName : ";
  remoteDeviceInfo+=serviceInfo.serviceName();
  remoteDeviceInfo+="\n";
  remoteDeviceInfo+="serviceDescription : ";
  remoteDeviceInfo+=serviceInfo.attribute(QBluetoothServiceInfo::ServiceDescription).toString();
  remoteDeviceInfo+="\n";
  remoteDeviceInfo+="serverChannel : ";
  remoteDeviceInfo+=serviceInfo.serverChannel();
  remoteDeviceInfo+="\n";
  remoteDeviceInfo+="serviceClassUuids : ??";
  //remoteDeviceInfo+=serviceInfo.serviceClassUuids();
  remoteDeviceInfo+="\n";
  remoteDeviceInfo+="serviceProvider : ";
  remoteDeviceInfo+=serviceInfo.attribute(QBluetoothServiceInfo::ServiceProvider).toString();
  remoteDeviceInfo+="\n";
  remoteDeviceInfo+="serviceUuid: ??";
  //remoteDeviceInfo+=serviceInfo.serviceUuid();
  remoteDeviceInfo+="\n";
  remoteDeviceInfo+="attributes: ??";
  //remoteDeviceInfo+=serviceInfo.attributes();
  remoteDeviceInfo+="\n";
  remoteDeviceInfo+="device : ??";
  //remoteDeviceInfo+=serviceInfo.device();
  remoteDeviceInfo+="\n";
  remoteDeviceInfo+="isComplete : ";
  remoteDeviceInfo+=serviceInfo.isComplete();
  remoteDeviceInfo+="\n";
  remoteDeviceInfo+="isRegistered: ";
  remoteDeviceInfo+=serviceInfo.isRegistered();
  remoteDeviceInfo+="\n";
  remoteDeviceInfo+="isValid : ";
  remoteDeviceInfo+=serviceInfo.isValid();

  spSharedData->log->logDebbug(remoteDeviceInfo);

  ui->textEdit->setText("service discovery: \n"+serviceInfo.serviceName());
}


void BtConnector::on_buttonPair_clicked()
{
  spSharedData->log->logDebbug("Pair button click");
  if(spSharedData->localDevice->isValid()){
    spSharedData->log->logDebbug("PAIR: spSharedData->localDevice is valid");
    QBluetoothDeviceInfo selectedRemoteDevice = getSelectedRemoteDevice();
        if(selectedRemoteDevice.isValid()){
        spSharedData->log->logDebbug("PAIR: selectedRemoteDev is VALID & selectedRemoteDevName: "+selectedRemoteDevice.name() +" selectedRemoteDevAddr: "+selectedRemoteDevice.address().toString());
        QBluetoothLocalDevice::Pairing stat = spSharedData->localDevice->pairingStatus(selectedRemoteDevice.address());
        QString sta= (stat== QBluetoothLocalDevice::Unpaired? "true" :"false");
        spSharedData->log->logDebbug("PAIR: selectedRemoteDev pairing status is unpaired ? : " + sta);
        if(stat== QBluetoothLocalDevice::Unpaired){
          spSharedData->log->logDebbug("Request pairing with "+selectedRemoteDevice.name());
          spSharedData->localDevice->requestPairing(selectedRemoteDevice.address(),QBluetoothLocalDevice::Paired);
        }
        else{
            sta= (stat== QBluetoothLocalDevice::Paired? "true" :"false");
            spSharedData->log->logDebbug("PAIR: selectedRemoteDev pairing status is paired ? : " + sta);
            sta= (stat== QBluetoothLocalDevice::AuthorizedPaired? "true" :"false");
            spSharedData->log->logDebbug("PAIR: selectedRemoteDev pairing status is AuthPaired ? : " + sta);
            emit(spSharedData->localDevice->pairingFinished(selectedRemoteDevice.address(),stat));
        }

    }
  }
}
void BtConnector::on_buttonUnpair_clicked()
{
  if(spSharedData->localDevice->isValid()){
    QBluetoothDeviceInfo selectedRemoteDevice = getSelectedRemoteDevice();
    if(selectedRemoteDevice.isValid()){
        if(spSharedData->localDevice->pairingStatus(selectedRemoteDevice.address()) !=QBluetoothLocalDevice::Unpaired){
          spSharedData->log->logDebbug("Request unpairing with "+selectedRemoteDevice.name());
          spSharedData->localDevice->requestPairing(selectedRemoteDevice.address(),QBluetoothLocalDevice::Unpaired);
          }
    }
  }
}


QBluetoothDeviceInfo BtConnector::getSelectedRemoteDevice(){
  QBluetoothDeviceInfo selectedRemoteDevice;
  QList<QListWidgetItem *> items = ui->listDevicesNearby->selectedItems();
  if(items.size()==1){
      QList<QBluetoothDeviceInfo> list = localDiscoveryAgent->discoveredDevices();
      if(!list.isEmpty()){
        QListWidgetItem *item = items.first();
        for(auto el : list)
          if((el.name()+" | "+ el.address().toString() )==item->text()){
              selectedRemoteDevice=el;
            }
        }
    }
  spSharedData->log->logDebbug("selected device : "+selectedRemoteDevice.name());
  return selectedRemoteDevice;
}

void BtConnector::debugInfo(QString s)
{
   s="\n--------------\n" +s;
   ui->textEdit->append(s);
}

void BtConnector::finishPairing(QBluetoothAddress addr, QBluetoothLocalDevice::Pairing status){
  QList<QBluetoothDeviceInfo> list = localDiscoveryAgent->discoveredDevices();
  QString name ="";

  if(!list.isEmpty()){
      for(auto &a : list){
          if(a.address()==addr){
            name = a.name();
            break;
            }
        }
      spSharedData->log->logDebbug("PAIRING FINISH : paired with "+ name);
      if(!name.isEmpty()){
        QList<QListWidgetItem *> items = ui->listDevicesNearby->selectedItems();
        if(items.size()==1){
          QListWidgetItem *item = items.first();
          if(status == QBluetoothLocalDevice::Paired || status == QBluetoothLocalDevice::AuthorizedPaired){
              spSharedData->log->logDebbug("device paired");
            item->setBackgroundColor(QColor("green"));
          }
          else{
            item->setBackgroundColor(QColor("white"));
            spSharedData->log->logDebbug("Device unpaired");
          }
        }
      }
  }


}


void BtConnector::on_buttonConnect_clicked()
{
   spSharedData->log->logDebbug("connect device button clicked");
   spSharedData->test="onClick change by object ";
   selectedDevice = new QBluetoothDeviceInfo(getSelectedRemoteDevice());

   spSharedData->test="onClick change by sharedptr";

   QBluetoothDeviceInfo selectedRemoteDevice=*selectedDevice;
   spSharedData->mySocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
   connect(spSharedData->mySocket, SIGNAL(connected()),this,SLOT(socketConnected()));
   connect(spSharedData->mySocket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
   //connect(spSharedData->mySocket, SIGNAL(readyRead()), this, SLOT(socketRead()));
   connect(spSharedData->mySocket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(socketError(QBluetoothSocket::SocketError)));
   connect(this, SIGNAL(conectedToSocket(QBluetoothSocket*, QString)), this, SLOT(close()));
   //connect(spSharedData->mySocket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(close()));

   spSharedData->mySocket->connectToService(selectedRemoteDevice.address(),QBluetoothUuid::SerialPort , QIODevice::ReadWrite);

//Serial Port Profile - serviceName dev: ODBII
}


void BtConnector::socketConnected(){
  if(spSharedData->mySocket->state()==QBluetoothSocket::ConnectedState){
    emit(conectedToSocket(spSharedData->mySocket,selectedDevice->name()));
    // wyłączone - zmiana obsługi pisania do elm327 na klase exchanget
   /*
    //* zakomentowane żeby sprawdzić poprawność działania połączenia z socketem w obdScanerze a nie w connectorze
    dataEx = new ObdDataExchanger(spSharedData->mySocket,spSharedData->log);
    connect(spSharedData->mySocket, SIGNAL(readyRead()), dataEx, SLOT(getDataFromElm327()));
    */
    spSharedData->log->logDebbug("socket connected");
    debugInfo("connected");
  }
  else{
      spSharedData->log->logDebbug("socket status : not connected ");
    }
}

void BtConnector::socketDisconnected(){
  spSharedData->log->logDebbug("socket disconnected");
  if(spSharedData->mySocket)
    spSharedData->mySocket=nullptr;
}
void BtConnector::socketRead(){
     spSharedData->log->logDebbug("reading from socket");

     if(spSharedData->mySocket->bytesAvailable()>0){
        QByteArray line = spSharedData->mySocket->readLine();
        spSharedData->log->logInfo("Read FROM SOCKET : raw " + line);
        debugInfo("\nOdp :" + line);
     }
     spSharedData->log->logDebbug("reading finish- canReadLine)() false");

}
void BtConnector::socketError(QBluetoothSocket::SocketError){
  //debugInfo("err: " + spSharedData->mySocket->errorString());
  spSharedData->log->logDebbug("err: " + spSharedData->mySocket->errorString());
  emit(notConectedToSocket(selectedDevice));
  }

void BtConnector::on_buttonDisconnect_clicked()
{
    spSharedData->log->logDebbug("clicked disconnect buton");
    if(spSharedData->mySocket->isOpen() &&  spSharedData->mySocket->state() != QBluetoothSocket::UnconnectedState){
      spSharedData->mySocket->disconnectFromService();
      }
}


