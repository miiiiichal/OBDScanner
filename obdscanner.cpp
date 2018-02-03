#include "obdscanner.h"
#include "ui_obdscanner.h"
#include "btconnector.h"
#include <string>

OBDScanner::OBDScanner(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::OBDScanner)
{
  ui->setupUi(this);
  mySharedData = std::make_shared<DataKeeper>(new DataKeeper());
  mySharedData->test="text by shared in obdscanner constr";

  if(mySharedData->localDevice->hostMode()==QBluetoothLocalDevice::HostPoweredOff){
      ui->btConfigButton->setEnabled(false);
      ui->btRadioButton->setChecked(false);
    }

   //log= new Logger("./logs/OBDScannerLog.txt");
  mySharedData->log= new Logger("./logs/OBDScannerLog2.txt");
  //log->logInfo("Application statrs");
  mySharedData->log->logInfo("Application statrs");


}

OBDScanner::~OBDScanner()
{
  delete ui;
  mySharedData->log->logInfo("Application ends");
  //delete mySharedData->log;
}

void OBDScanner::on_btRadioButton_clicked(bool checked)
{
  if(checked){
      ui->btConfigButton->setEnabled(true);
      mySharedData->localDevice->powerOn();
    }
  else{
    ui->btConfigButton->setEnabled(false);
    mySharedData->localDevice->setHostMode(QBluetoothLocalDevice::HostPoweredOff);
    }
}

void OBDScanner::on_btConfigButton_clicked()
{
  //BtConnector btConnectorForm(mySharedData->localDevice, mySharedData->log);
   // BtConnector btConnectorForm(myData);
    BtConnector btConnectorForm(mySharedData);

  //connect(&btConnectorForm, SIGNAL(testSignal(QString)),this,SLOT(getSignalFromConnector(QString)));
  //connect(&btConnectorForm, SIGNAL(conectedToSocket(QBluetoothSocket *)),this,SLOT(getSocketFromConnector(QBluetoothSocket *)));
  connect(&btConnectorForm, SIGNAL(conectedToSocket(QBluetoothSocket *, QString)),this,SLOT(getSocketFromConnector(QBluetoothSocket *, QString)));
  connect(&btConnectorForm, SIGNAL(notConectedToSocket(QBluetoothDeviceInfo *)),this,SLOT(getDeviceInfoFromConnector(QBluetoothDeviceInfo *)));

//   btConnectorForm->setModal(true);
  btConnectorForm.exec();
}

void OBDScanner::getSocketFromConnector(QBluetoothSocket *mySocket, QString selectedDevName){
    mySharedData->log->logDebbug("get signal about connected socket ");
    if(mySocket->state()==QBluetoothSocket::ConnectedState){
        mySharedData->log->logDebbug("and got valid socket pointer, socket state : Connected");
        ui->connectedDeviceName->setText(selectedDevName);
        //dataExchanger=new ObdDataExchanger(mySocket,mySharedData->log);
        mySharedData->dataExchanger = new ObdDataExchanger(mySharedData->mySocket,mySharedData->log);
        if(mySocket==mySharedData->mySocket)
            mySharedData->log->logDebbug("socket returned from BtConnector by signal and socket stored in mySharedData are equal");

        connect(mySharedData->dataExchanger,SIGNAL(readDataReady(QString)),this,SLOT(obdResponseDispatcher(QString)));
    }
}
void OBDScanner::getDeviceInfoFromConnector(QBluetoothDeviceInfo *deviceInfo){
    mySharedData->log->logDebbug("get signal about NOTconnected socket - dev:" +deviceInfo->name());
    ui->connectedDeviceName->setText("Connot connect to "+deviceInfo->name());
}

void OBDScanner::getSignalFromConnector(QString str){
    mySharedData->log->logDebbug("get signal from btConnector :"+str);
    ui->connectedDeviceName->setText(str);
}


void OBDScanner::on_tabWidget_tabBarClicked(int index)
{
  mySharedData->log->logDebbug(QString("tab clicked -  idx: "+QString::number(index) ) );
  activeTab = index;
  switch(activeTab){
      case ActiveTab::Info :
      {
          mySharedData->log->logDebbug(QString("query from INFO"));
          ui->info_testEdit->setText(QString(""));
          //get car info

          break;
      }
      case ActiveTab::Rtd :
      {
          mySharedData->log->logDebbug(QString("query from RTD"));
          ui->rtd_testEdit->setText(QString("rtd"));
          getRTD();
          break;
      }
      case ActiveTab::Dtc :
      {
          mySharedData->log->logDebbug(QString("response to DTC"));
          ui->dtc_testEdit->setText(QString("odpowiedz do tej zakładki dtc"));
          break;
      }
      case ActiveTab::Cmd :
      {
          mySharedData->log->logDebbug(QString("query from CMD"));
         // ui->cmd_console->append();
          break;
      }
  }
}

void OBDScanner::on_cmd_clearButton_clicked()
{
    ui->cmd_input->setText("");
    mySharedData->log->logDebbug(QString("Clearing cmd input"));
}

void OBDScanner::on_cmd_sendButton_clicked()
{
    QString cmd = ui->cmd_input->text();
    if(!cmd.isEmpty()){
        mySharedData->dataExchanger->sendDataToElm327(cmd);
        ui->cmd_console->append("> "+cmd);
        mySharedData->log->logInfo(cmd);
    }
}

void OBDScanner::obdResponseDispatcher(const QString response)
{
    mySharedData->log->logDebbug(QString("Response dispatcher"));
    switch(activeTab){
        case ActiveTab::Info :
        {
            mySharedData->log->logDebbug(QString("response to INFO"));
            ui->info_testEdit->setText(QString("odpowiedz do zakładki info"));
            //get car info
            break;
        }
        case ActiveTab::Rtd :
        {
            mySharedData->log->logDebbug(QString("response to RTD"));
            ui->rtd_testEdit->setText(QString("odpowiedz do zakładki rtc"));
            break;
        }
        case ActiveTab::Dtc :
        {
            mySharedData->log->logDebbug(QString("response to DTC"));
            ui->dtc_testEdit->setText(QString("odpowiedz do tej zakładki dtc"));
            break;
        }
        case ActiveTab::Cmd :
        {
            mySharedData->log->logDebbug(QString("response to CMD"));
            ui->cmd_console->append(response);
            break;
        }
    }
}

void OBDScanner::obdResponseDispatcher()
{

}

void OBDScanner::getRTD()
{
    //current speed, rpm, coolant temp, intake temp
    int counter=0;
    mySharedData->log->logDebbug("starting RTD reading");
    while(activeTab==ActiveTab::Rtd){
        //bezpiecznik
        if(counter>100)
            break;
        else
            ++counter;

        mySharedData->dataExchanger->sendDataToElm327(QString("01 0D"));
        mySharedData->dataExchanger->sendDataToElm327(QString("01 0C"));
        mySharedData->dataExchanger->sendDataToElm327(QString("01 05"));

    }
}
