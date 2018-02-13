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
  mySharedData->dataExchanger->setContinueRequesting(false);
  mySharedData->dataExchanger->clearRegisters();
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
          getRTD();
          break;
      }
      case ActiveTab::Dtc :
      {
          mySharedData->log->logDebbug(QString("response to DTC"));
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
    ObdDataParser dataParser;
    std::vector<QString> resp;
    switch(activeTab){
        case ActiveTab::Info :
        {

            mySharedData->log->logDebbug(QString("response to INFO"));
            ui->info_testEdit->setText(QString("odpowiedz do zakładki info"));
            //get car info : fuel type,battery voltage, obd type, vin , etc.
            break;
        }
        case ActiveTab::Rtd :
        {
            resp= dataParser.prepareResponseToDecode(response);
            if(!resp[0].compare(QString("NO DATA"),Qt::CaseInsensitive))
                return;
            mySharedData->log->logDebbug(QString("response to RTD"));
            if(resp.size()>2 && !resp[2].compare("41",Qt::CaseInsensitive)){
                int pid =std::stoi(resp[3].toStdString(),nullptr,16);
                std::vector<QString> vec;
                vec.insert(vec.begin(),resp.begin()+4, resp.end());
                switch(pid){
                    case 5:{
                        //coolant
                        int temp = dataParser.decodeCoolantTemp(vec);
                        ui->coolantLcd->display(temp);
                        ui->coolant_dial->setValue(temp);
                        break;
                    }
                    case 12:{
                        //rpm;
                        int rpm = dataParser.decodeEngineRPM(vec);
                        ui->rpmLcd->display(rpm);
                        ui->rpm_dial->setValue(rpm);
                        break;
                    }
                    case 13 :{
                        //speed
                        int speed = dataParser.decodeKmHSpeed(vec);
                        ui->speedLcd->display(speed);
                        ui->speed_dial->setValue(speed);

                        break;
                    }
                    case 15 : {
                        //intake temp;
                        int intake_temp = dataParser.decodeIntakeAirTemp(vec);
                        ui->inTempLcd->display(intake_temp);
                        ui->intake_dial->setValue(intake_temp);
                        break;
                    }
                }
            }
            break;
        }
        case ActiveTab::Dtc :
        {
            //0101 -> 41 01 82 00 00 00
        //03 --
            mySharedData->log->logDebbug(QString("response to DTC"));
            std::vector<QString> vec;
            resp= dataParser.prepareResponseToDecode(response);
            if(!resp[0].compare(QString("NO DATA"),Qt::CaseInsensitive)){
                return;
            }

            if(resp.size()>2 && !resp[2].compare("41",Qt::CaseInsensitive)){
                vec.insert(vec.begin(),resp.begin()+4, resp.end());
                std::pair<int,bool> dtcNumber = dataParser.decodeNumberOfDtc(vec);
                ui->dtc_numberEdit->setText(QString::number(dtcNumber.first));
                ui->dtc_milIndicatorON->setChecked(dtcNumber.second);
            }
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
 //   while(activeTab==ActiveTab::Rtd){
        //bezpiecznik
        //if(counter>100)
            //break;
       // else
      //      ++counter;
    mySharedData->dataExchanger->setContinueRequesting(true);
    mySharedData->dataExchanger->sendDataToElm327(QString("01 0F"));
    mySharedData->dataExchanger->sendDataToElm327("01 0D");
    mySharedData->dataExchanger->sendDataToElm327(QString("01 0C"));
    mySharedData->dataExchanger->sendDataToElm327(QString("01 05"));

  //  }
}

void OBDScanner::on_pushButton_clicked()
{
   obdResponseDispatcher(QString("41 0D 0C 00"));
   obdResponseDispatcher(QString("41 0D 0d 00"));
   obdResponseDispatcher(QString("41 0D 0E 00"));
   obdResponseDispatcher(QString("41 0D 0F 00"));
   obdResponseDispatcher(QString("41 0D 10 00"));
   obdResponseDispatcher(QString("41 0C 0C 2E"));
   obdResponseDispatcher(QString("41 05 0C 2E"));
}
