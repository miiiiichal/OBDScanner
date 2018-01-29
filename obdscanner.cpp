#include "obdscanner.h"
#include "ui_obdscanner.h"
#include "btconnector.h"
#include <string>

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

  /*btConnectorForm = new BtConnector(localDevice, log);

  connect(btConnectorForm, SIGNAL(testSignal(QString)),this,SLOT(getSignalFromConnector(QString)));
  connect(btConnectorForm, SIGNAL(conectedToSocket(QBluetoothSocket*)),this,SLOT(getSocketFromConnector(QBluetoothSocket*)));
  connect(btConnectorForm, SIGNAL(notConectedToSocket(QBluetoothDeviceInfo *)),this,SLOT(getDeviceInfoFromConnector(QBluetoothDeviceInfo*)));
*/
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

  connect(&btConnectorForm, SIGNAL(testSignal(QString)),this,SLOT(getSignalFromConnector(QString)));
  connect(&btConnectorForm, SIGNAL(conectedToSocket(QBluetoothSocket *)),this,SLOT(getSocketFromConnector(QBluetoothSocket *)));
  connect(&btConnectorForm, SIGNAL(notConectedToSocket(QBluetoothDeviceInfo *)),this,SLOT(getDeviceInfoFromConnector(QBluetoothDeviceInfo *)));

//   btConnectorForm->setModal(true);
  btConnectorForm.exec();
}

void OBDScanner::getSocketFromConnector(QBluetoothSocket *mySocket){
    log->logDebbug("get signal about connected socket ");
    if(mySocket->state()==QBluetoothSocket::ConnectedState){
        log->logDebbug("and got valid socket pointer");
        ui->connectedDeviceName->setText(mySocket->peerName());
        dataExchanger=new ObdDataExchanger(mySocket,log);

        connect(dataExchanger,SIGNAL(readDataReady(QString)),this,SLOT(obdResponseDispatcher(QString)));
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


void OBDScanner::on_tabWidget_tabBarClicked(int index)
{
  log->logDebbug(QString("tab clicked -  idx: "+QString::number(index) ) );
  activeTab = index;
}

void OBDScanner::on_cmd_clearButton_clicked()
{
    ui->cmd_input->setText("");
    log->logDebbug(QString("Clearing cmd input"));
}

void OBDScanner::on_cmd_sendButton_clicked()
{
    QString cmd = ui->cmd_input->text();
    if(!cmd.isEmpty()){
        dataExchanger->sendDataToElm327(cmd);
        ui->cmd_console->append("> "+cmd);
        log->logInfo(cmd);
    }
}

void OBDScanner::obdResponseDispatcher(const QString response)
{
    log->logDebbug(QString("Response dispatcher"));
    switch(activeTab){
        case ActiveTab::Info :
        {
            log->logDebbug(QString("response to INFO"));
            ui->info_testEdit->setText(QString("odpowiedz do zakładki info"));
            break;
        }
        case ActiveTab::Rtd :
        {
            log->logDebbug(QString("response to RTD"));
            ui->rtd_testEdit->setText(QString("odpowiedz do zakładki rtc"));
            break;
        }
        case ActiveTab::Dtc :
        {
            log->logDebbug(QString("response to DTC"));
            ui->dtc_testEdit->setText(QString("odpowiedz do tej zakładki dtc"));
            break;
        }
        case ActiveTab::Cmd :
        {
            log->logDebbug(QString("response to CMD"));
            ui->cmd_console->append(response);
            break;
        }
    }
}

void OBDScanner::obdResponseDispatcher()
{

}
