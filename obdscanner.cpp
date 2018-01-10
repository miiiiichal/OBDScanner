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

  log= new Logger("OBDScannerLog.txt");
  log->logInfo("Logger Start");
  QString ss = "logggggggggggeeeerrrrr info";
  log->logInfo(ss);
  log->logDebbug("debbug log sssssssss");
  log->logError("errorrek");
}

OBDScanner::~OBDScanner()
{
  delete ui;

  //qt git remote commit test testhtml
  //next test
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
  BtConnector btConnectorForm(localDevice);
  btConnectorForm.exec();

  connect(&btConnectorForm, SIGNAL(mySignal(QBluetoothSocket *socket)),this,SLOT(getSignalFromConnector(QBluetoothSocket *socket)));
}
void OBDScanner::getSignalFromConnector(QBluetoothSocket *mySocket){
    log->logDebbug("get signal from socket ");
    if(mySocket->state()==QBluetoothSocket::ConnectedState)
        log->logDebbug("and got valid socket pointer");
}
