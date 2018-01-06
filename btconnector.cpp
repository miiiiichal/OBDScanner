#include "btconnector.h"
#include "ui_btconnector.h"


BtConnector::BtConnector(QBluetoothLocalDevice &localDev, QWidget *parent) :
  QDialog(parent),
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
}

BtConnector::~BtConnector()
{
  delete localDiscoveryAgent;

  delete serviceDiscoveryAgent;
  delete ui;
}



void BtConnector::on_buttonScan_clicked()
{
    startScanning();
}

void BtConnector::startScanning(){

  localDiscoveryAgent->start();
  ui->buttonScan->setEnabled(false);

  }

void BtConnector::stopScanning(){

  localDiscoveryAgent->stop();
  ui->buttonScan->setEnabled(true);

}
void BtConnector::addFoundDevice(QBluetoothDeviceInfo deviceInfo){
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

void BtConnector::doSth(QBluetoothServiceInfo &info){
   ui->textEdit->setText(tr("ghjkl"));
}

void BtConnector::serviceDiscovered(const QBluetoothServiceInfo &serviceInfo){
 //  ui->textEdit->setText("remoteServiceInfo");
  QString remoteDeviceInfo;

  remoteDeviceInfo.clear();
  remoteDeviceInfo+="serviceName : ";
  remoteDeviceInfo+=serviceInfo.serviceName();
  remoteDeviceInfo+="\n---------------------------\n";
  remoteDeviceInfo+="serviceDescription : ";
  remoteDeviceInfo+=serviceInfo.serviceDescription();
  remoteDeviceInfo+="\n---------------------------\n";
  remoteDeviceInfo+="serverChannel : ";
  remoteDeviceInfo+=serviceInfo.serverChannel();
  remoteDeviceInfo+="\n---------------------------\n";
  remoteDeviceInfo+="serviceClassUuids : ??";
  //remoteDeviceInfo+=serviceInfo.serviceClassUuids();
  remoteDeviceInfo+="\n---------------------------\n";
  remoteDeviceInfo+="serviceProvider : ";
  remoteDeviceInfo+=serviceInfo.serviceProvider();
  remoteDeviceInfo+="\n---------------------------\n";
  remoteDeviceInfo+="serviceUuid: ??";
  //remoteDeviceInfo+=serviceInfo.serviceUuid();
  remoteDeviceInfo+="\n---------------------------\n";
  remoteDeviceInfo+="attributes: ??";
  //remoteDeviceInfo+=serviceInfo.attributes();
  remoteDeviceInfo+="\n---------------------------\n";
  remoteDeviceInfo+="device : ??";
  //remoteDeviceInfo+=serviceInfo.device();
  remoteDeviceInfo+="\n---------------------------\n";
  remoteDeviceInfo+="isComplete : ";
  remoteDeviceInfo+=serviceInfo.isComplete();
  remoteDeviceInfo+="\n---------------------------\n";
  remoteDeviceInfo+="isRegistered: ";
  remoteDeviceInfo+=serviceInfo.isRegistered();
  remoteDeviceInfo+="\n---------------------------\n";
  remoteDeviceInfo+="isValid : ";
  remoteDeviceInfo+=serviceInfo.isValid();



  ui->textEdit->setText(remoteDeviceInfo);
}


void BtConnector::on_buttonPair_clicked()
{
  if(localDevice->isValid()){
    QBluetoothDeviceInfo selectedRemoteDevice = getSelectedRemoteDevice();
    if(selectedRemoteDevice.isValid()){
        if(localDevice->pairingStatus(selectedRemoteDevice.address())==0){
          localDevice->requestPairing(selectedRemoteDevice.address(),QBluetoothLocalDevice::Paired);
          }
    }
  }
}
void BtConnector::on_buttonUnpair_clicked()
{
  if(localDevice->isValid()){
    QBluetoothDeviceInfo selectedRemoteDevice = getSelectedRemoteDevice();
    if(selectedRemoteDevice.isValid()){
        if(localDevice->pairingStatus(selectedRemoteDevice.address())>0){
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
  return selectedRemoteDevice;
}

void BtConnector::finishPairing(QBluetoothAddress addr,QBluetoothLocalDevice::Pairing status){
  QList<QBluetoothDeviceInfo> list = localDiscoveryAgent->discoveredDevices();
  QString name ="";
  if(!list.isEmpty()){
      for(auto &a : list){
          if(a.address()==addr){
            name = a.name();
            break;
            }
        }
      if(!name.isEmpty()){
        QList<QListWidgetItem *> items = ui->listDevicesNearby->selectedItems();
        if(items.size()==1){
          QListWidgetItem *item = items.first();
          if(status == QBluetoothLocalDevice::Paired || status == QBluetoothLocalDevice::AuthorizedPaired)
            item->setBackgroundColor(QColor("green"));
          else
            item->setBackgroundColor(QColor("white"));
        }
      }
  }


}

