#include "obdscanner.h"
#include "ui_obdscanner.h"
#include "btconnector.h"

OBDScanner::OBDScanner(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::OBDScanner)
{
  ui->setupUi(this);

  if(localDevice.hostMode()==QBluetoothLocalDevice::HostPoweredOff){
      ui->btConfigButton->setEnabled(false);
      ui->btRadioButton->setChecked(false);
    }

  log= new Logger("./logs/OBDScannerLog.txt");
  log->logInfo("Application statrs");

}

OBDScanner::~OBDScanner()
{
  delete ui;
  log->logInfo("Application ends");
  delete log;
}

void OBDScanner::on_btRadioButton_clicked(bool checked)
{
  if(checked){
      ui->btConfigButton->setEnabled(true);
      localDevice.powerOn();
    }
  else{
    ui->btConfigButton->setEnabled(false);
    localDevice.setHostMode(QBluetoothLocalDevice::HostPoweredOff);
    }
}

void OBDScanner::on_btConfigButton_clicked()
{
  BtConnector btConnectorForm(localDevice, log);

  connect(&btConnectorForm, SIGNAL(testSignal(QString str)),this,SLOT(getSignalFromConnector(QString str)));
  connect(&btConnectorForm, SIGNAL(conectedToSocket(QBluetoothSocket *socket)),this,SLOT(getSocketFromConnector(QBluetoothSocket *socket)));
  connect(&btConnectorForm, SIGNAL(notConectedToSocket(QBluetoothDeviceInfo *deviceInfo)),this,SLOT(getDeviceInfoFromConnector(QBluetoothDeviceInfo *device)));
  btConnectorForm.exec();
}

void OBDScanner::getSocketFromConnector(QBluetoothSocket *mySocket){
    log->logDebbug("get signal about connected socket ");
    if(mySocket->state()==QBluetoothSocket::ConnectedState){
        log->logDebbug("and got valid socket pointer");
        ui->connectedDeviceName->setText(mySocket->peerName());
    }
}
void OBDScanner::getDeviceInfoFromConnector(QBluetoothDeviceInfo *deviceInfo){
    log->logDebbug("get signal about NOTconnected socket - dev:" +deviceInfo->name());
    ui->connectedDeviceName->setText("Connot connect to "+deviceInfo->name());
}

void OBDScanner::getSignalFromConnector(QString str){
    log->logDebbug("get signal from btConnector :"+str);
    ui->connectedDeviceName->setText(str);
}
