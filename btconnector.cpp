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
  connect(serviceDiscoveryAgent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)), this, SLOT(serviceDiscovered(QBluetoothServiceInfo)));

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
  QListWidgetItem *foundDeviceName = new QListWidgetItem(deviceInfo.name() + " | " +deviceInfo.address().toString());
  QList<QListWidgetItem *> items = ui->listDevicesNearby->findItems(deviceInfo.name() + " | " +deviceInfo.address().toString(), Qt::MatchExactly );
  if(items.empty())
    ui->listDevicesNearby->addItem(foundDeviceName);
}

void BtConnector::on_buttonRemoteDeviceInfo_clicked()
{
    QList<QListWidgetItem *> items = ui->listDevicesNearby->selectedItems();
    if(!items.isEmpty() && items.size()==1){
        QList<QBluetoothDeviceInfo> list = localDiscoveryAgent->discoveredDevices();
        if(!list.isEmpty()){
          QListWidgetItem *item = items.first();
          for(auto el : list)
            if((el.name()+" | "+ el.address().toString() )==item->text()){
                QBluetoothAddress remoteAddr = el.address();
                QString str(el.address().toString()+" | "+el.name());
                 ui->textEdit->setText(str);
                serviceDiscoveryAgent->setRemoteAddress(remoteAddr);
                serviceDiscoveryAgent->start();

            }
        }
      }
}

void BtConnector::doSth(QBluetoothServiceInfo &info){
   ui->textEdit->setText(tr("ghjkl"));
}

void BtConnector::serviceDiscovered(const QBluetoothServiceInfo &serviceInfo){
 //  ui->textEdit->setText("remoteServiceInfo");
  QString remoteDeviceInfo;

  remoteDeviceInfo.clear();
  remoteDeviceInfo+=serviceInfo.serviceName();
  remoteDeviceInfo+="\n"+serviceInfo.serviceDescription();

  ui->textEdit->setText(remoteDeviceInfo);
}

void BtConnector::on_buttonConnect_clicked()
{

}
