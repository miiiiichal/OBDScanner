#include "btconnector.h"
#include "ui_btconnector.h"

BtConnector::BtConnector(QBluetoothLocalDevice &localDev, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::BtConnector)
{
  // class members
  ui->setupUi(this);

  localDevAddr = localDev.address();
  localDiscoveryAgent  = new QBluetoothDeviceDiscoveryAgent();
  serviceDiscoveryAgent = new QBluetoothServiceDiscoveryAgent(localDevAddr);

  //interface init vals
  ui->lineEdit_address->setText(localDevAddr.toString());
  ui->lineEdit_name->setText(localDev.name());
  setWindowTitle("Bluetooth device management dialog");

  // my Signal->Slot actions
  connect(localDiscoveryAgent , SIGNAL(finished()) , this , SLOT(stopScanning()));
  connect(localDiscoveryAgent , SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)) , this , SLOT(addFoundDevice(QBluetoothDeviceInfo)));
  connect(serviceDiscoveryAgent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)), this, SLOT(doSth()));

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
  QListWidgetItem *foundDeviceName = new QListWidgetItem(deviceInfo.name());
  QList<QListWidgetItem *> items = ui->listDevicesNearby->findItems(deviceInfo.name(), Qt::MatchExactly );
  if(items.empty())
    ui->listDevicesNearby->addItem(foundDeviceName);

}

void BtConnector::on_buttonRemoteDeviceInfo_clicked()
{
    QList<QListWidgetItem *> items = ui->listDevicesNearby->selectedItems();
    if(!items.isEmpty() && items.size()==1){
        // show device info
        //ui->listDevicesNearby->clear();
        //ui->listDevicesNearby->removeItemWidget(items.first());
        QList<QBluetoothDeviceInfo> list = localDiscoveryAgent->discoveredDevices();
        if(!list.isEmpty()){
          QListWidgetItem *item = items.first();
          for(auto el : list)
            if(el.name()==item->text()){
              //serviceDiscoveryAgent = new QBluetoothServiceDiscoveryAgent(localDevAddr);
                QBluetoothAddress remoteAddr = el.address();
                serviceDiscoveryAgent->setRemoteAddress(remoteAddr);

                QList<QBluetoothServiceInfo> discoveredServices = serviceDiscoveryAgent->discoveredServices();
                //nie znajduje serwisu - lista pusta
                if(!discoveredServices.isEmpty()){
                    QString remoteDeviceInfo;
                    for (auto service : discoveredServices){
                        remoteDeviceInfo.clear();
                        remoteDeviceInfo+=service.serviceName();
                        remoteDeviceInfo+="\n"+service.serviceDescription();
                      }
                    ui->textEdit->setText(remoteDeviceInfo);
                }

            }
        }
      }
}

void BtConnector::doSth(){
   ui->textEdit->setText(tr("ghjkl"));
}
