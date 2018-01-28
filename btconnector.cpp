#include "btconnector.h"
#include "logger.h"
#include "ui_btconnector.h"

#include <iostream>
#include <string>
#include <sstream>


BtConnector::BtConnector(QBluetoothLocalDevice &localDev, Logger *log, QWidget *parent) :
  QDialog(parent),
  log(log),
  ui(new Ui::BtConnector)
{
  // class members
  ui->setupUi(this);

  localDevice = &localDev;
  localDevAddr = localDev.address();
  localDiscoveryAgent  = new QBluetoothDeviceDiscoveryAgent();
  serviceDiscoveryAgent = new QBluetoothServiceDiscoveryAgent(localDevAddr);


  //interface init vals
  ui->lineEdit_address->setText(localDevAddr.toString());
  ui->lineEdit_name->setText(localDev.name());
  setWindowTitle("Bluetooth device management dialog");

  // my Signal->Slot actions
  connect(ui->buttonClose, SIGNAL(clicked()),this,SLOT(close()) );
  connect(localDiscoveryAgent , SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)) , this , SLOT(addFoundDevice(QBluetoothDeviceInfo)));
  connect(localDiscoveryAgent , SIGNAL(finished()) , this , SLOT(stopScanning()));
  connect(serviceDiscoveryAgent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)), this, SLOT(serviceDiscovered(QBluetoothServiceInfo)));
  connect(&localDev, SIGNAL(pairingDisplayPinCode(QBluetoothAddress,QString)), this, SLOT(displayPin(QBluetoothAddress,QString)));
  connect(&localDev,SIGNAL(pairingDisplayConfirmation(QBluetoothAddress,QString)),&localDev,SLOT(pairingConfirmation(bool)) );
  connect(&localDev,SIGNAL(pairingFinished(QBluetoothAddress,QBluetoothLocalDevice::Pairing)),this,SLOT(finishPairing(QBluetoothAddress,QBluetoothLocalDevice::Pairing)) );

  connect(this, SIGNAL(testSignal(QString)), this,SLOT(localSignalCatcher(QString)));
  log->logDebbug("btConnector created");


}

BtConnector::~BtConnector()
{
  delete localDiscoveryAgent;
  delete serviceDiscoveryAgent;
  delete ui;
  log->logDebbug("btConnector destroyed");
}


void BtConnector::on_buttonScan_clicked()
{
    startScanning();
}

void BtConnector::startScanning(){

  localDiscoveryAgent->start();
  ui->buttonScan->setEnabled(false);
  log->logDebbug("Scanning BT devices statrs");
  }

void BtConnector::stopScanning(){

  localDiscoveryAgent->stop();
  ui->buttonScan->setEnabled(true);
  log->logDebbug("Scanning BT devices finished");
}
void BtConnector::addFoundDevice(QBluetoothDeviceInfo deviceInfo){
  log->logDebbug("Found device: "+deviceInfo.name() +" : "+deviceInfo.address().toString());
  if(deviceInfo.isValid() && !deviceInfo.isCached()){
    QListWidgetItem *foundDeviceName = new QListWidgetItem(deviceInfo.name() + " | " +deviceInfo.address().toString());
    if(localDevice->pairingStatus(deviceInfo.address())==1)
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

  log->logDebbug(remoteDeviceInfo);

  ui->textEdit->setText("service discovery: \n"+remoteDeviceInfo);
}


void BtConnector::on_buttonPair_clicked()
{
  log->logDebbug("Pair button click");
  if(localDevice->isValid()){
    log->logDebbug("PAIR: localDevice is valid");
    QBluetoothDeviceInfo selectedRemoteDevice = getSelectedRemoteDevice();
        if(selectedRemoteDevice.isValid()){
        log->logDebbug("PAIR: selectedRemoteDev is VALID & selectedRemoteDevName: "+selectedRemoteDevice.name() +" selectedRemoteDevAddr: "+selectedRemoteDevice.address().toString());
        QBluetoothLocalDevice::Pairing stat = localDevice->pairingStatus(selectedRemoteDevice.address());
        QString sta= (stat== QBluetoothLocalDevice::Unpaired? "true" :"false");
        log->logDebbug("PAIR: selectedRemoteDev pairing status is unpaired ? : " + sta);
        if(stat== QBluetoothLocalDevice::Unpaired){
          log->logDebbug("Request pairing with "+selectedRemoteDevice.name());
          localDevice->requestPairing(selectedRemoteDevice.address(),QBluetoothLocalDevice::Paired);
        }
        else{
            sta= (stat== QBluetoothLocalDevice::Paired? "true" :"false");
            log->logDebbug("PAIR: selectedRemoteDev pairing status is paired ? : " + sta);
            sta= (stat== QBluetoothLocalDevice::AuthorizedPaired? "true" :"false");
            log->logDebbug("PAIR: selectedRemoteDev pairing status is AuthPaired ? : " + sta);
            emit(localDevice->pairingFinished(selectedRemoteDevice.address(),stat));
        }

    }
  }
}
void BtConnector::on_buttonUnpair_clicked()
{
  if(localDevice->isValid()){
    QBluetoothDeviceInfo selectedRemoteDevice = getSelectedRemoteDevice();
    if(selectedRemoteDevice.isValid()){
        if(localDevice->pairingStatus(selectedRemoteDevice.address()) !=QBluetoothLocalDevice::Unpaired){
          log->logDebbug("Request unpairing with "+selectedRemoteDevice.name());
          localDevice->requestPairing(selectedRemoteDevice.address(),QBluetoothLocalDevice::Unpaired);
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
  log->logDebbug("selected device : "+selectedRemoteDevice.name());
  emit(testSignal(selectedRemoteDevice.name()));
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
      log->logDebbug("PAIRING FINISH : paired with "+ name);
      if(!name.isEmpty()){
        QList<QListWidgetItem *> items = ui->listDevicesNearby->selectedItems();
        if(items.size()==1){
          QListWidgetItem *item = items.first();
          if(status == QBluetoothLocalDevice::Paired || status == QBluetoothLocalDevice::AuthorizedPaired){
              log->logDebbug("device paired");
            item->setBackgroundColor(QColor("green"));
          }
          else{
            item->setBackgroundColor(QColor("white"));
            log->logDebbug("Device unpaired");
          }
        }
      }
  }


}


void BtConnector::on_buttonConnect_clicked()
{
   log->logDebbug("connect device button clicked");
   selectedDevice = new QBluetoothDeviceInfo(getSelectedRemoteDevice());
   QBluetoothDeviceInfo selectedRemoteDevice=*selectedDevice;
   mySocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);
   connect(mySocket, SIGNAL(connected()),this,SLOT(socketConnected()));
   connect(mySocket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
   connect(mySocket, SIGNAL(readyRead()), this, SLOT(socketRead()));
   connect(mySocket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(socketError(QBluetoothSocket::SocketError)));
   //connect(this, SIGNAL(conectedToSocket(QBluetoothSocket*)), this, SLOT(close()));
   //connect(mySocket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(close()));
   //connect(this, SIGNAL(testSignal(QString)), this, SLOT(close()));

   mySocket->connectToService(selectedRemoteDevice.address(),QBluetoothUuid::SerialPort , QIODevice::ReadWrite);


//Serial Port Profile - serviceName dev: ODBII
}


void BtConnector::socketConnected(){
  if(mySocket->state()==QBluetoothSocket::ConnectedState){
    //emmit() // emit signal connected socket
    emit(conectedToSocket(mySocket));
    // wyłączone - komunikacja przeniesiona do okna gł.
    log->logDebbug("socket connected");
    debugInfo("connected");
  }
  else{
      log->logDebbug("socket status : not connected ");
    }
}

void BtConnector::socketDisconnected(){
  log->logDebbug("socket disconnected");
  if(mySocket)
    delete mySocket;
}
void BtConnector::socketRead(){
     log->logDebbug("reading from socket");

     if(mySocket->bytesAvailable()>0){
        QByteArray line = mySocket->readLine();
        log->logInfo("Read FROM SOCKET : raw " + line);
        debugInfo("\nOdp :" + line);
     }
     log->logDebbug("reading finish- canReadLine)() false");

}
void BtConnector::socketError(QBluetoothSocket::SocketError){
  //debugInfo("err: " + mySocket->errorString());
  log->logDebbug("err: " + mySocket->errorString());
  emit(notConectedToSocket(selectedDevice));
  }

void BtConnector::on_buttonDisconnect_clicked()
{
    log->logDebbug("clicked disconnect buton");
    if(mySocket->isOpen() &&  mySocket->state() != QBluetoothSocket::UnconnectedState){
      mySocket->disconnectFromService();
      }
}


void BtConnector::on_pushButton_clicked()
{
    QString instruction(ui->inputInstr->text());
    instruction.append("\r");
    QByteArray buffer(instruction.toStdString().c_str());
    if(mySocket->isWritable())
        mySocket->write(buffer);
    log->logInfo("write instr: "+instruction);
    debugInfo("send :" + instruction);
   // while (mySocket->canReadLine()) {
   // QByteArray line = this->mySocket->readAll();
   // log->logDebbug("ReadFROMSOCKET after write: "+QString::fromUtf8(line.constData(), line.length()));
   // }
}

void BtConnector::on_pushButton_2_clicked()
{
    log->logDebbug("emitting testSignal ");
    emit testSignal(QString("ssssssssss"));
    log->logDebbug("emitting connectedToSocket ");
    emit(conectedToSocket(new QBluetoothSocket()));
}

void BtConnector::localSignalCatcher(QString s){
    log->logDebbug("catching test signal in btconnector : "+ s);
}
