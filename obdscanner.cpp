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

   //log= new Logger("./OBDScannerLog.txt");
  mySharedData->log= new Logger("OBDScannerLog.txt");
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
        mySharedData->dataExchanger->sendDataToElm327("AT SP 0");
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
  if(mySharedData->mySocket!=nullptr && mySharedData->mySocket->state()== QBluetoothSocket::ReadWrite){
      mySharedData->dataExchanger->setContinueRequesting(false);
      mySharedData->dataExchanger->clearRegisters();
      activeTab = index;
      switch(activeTab){
          case ActiveTab::Info :
          {
              mySharedData->log->logDebbug(QString("query from INFO"));

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
}

void OBDScanner::on_cmd_clearButton_clicked()
{
    if(activeTab==-1)
        activeTab=2;
    ui->cmd_input->setText("");
    mySharedData->log->logDebbug(QString("Clearing cmd input"));
}

void OBDScanner::on_cmd_sendButton_clicked()
{
    if(activeTab==-1)
        activeTab=2;
    QString cmd = ui->cmd_input->text();
    if(mySharedData->dataExchanger!=nullptr && !cmd.isEmpty()){
        mySharedData->dataExchanger->sendDataToElm327(cmd);
        ui->cmd_console->append("> "+cmd);
        mySharedData->log->logInfo(cmd);
    }
}

void OBDScanner::obdResponseDispatcher(const QString response)
{
    mySharedData->log->logDebbug(QString("Response dispatcher"));
    //ObdDataParser dataParser;
    std::vector<QString> resp;
    switch(activeTab){
        case ActiveTab::Info :
        {

            mySharedData->log->logDebbug(QString("response to INFO"));

            //get car info : fuel type,battery voltage, obd type, vin , etc.
            break;
        }
        case ActiveTab::Rtd :
        {
            resp= mySharedData->dataParser.prepareResponseToDecode(response);
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
                        int temp = mySharedData->dataParser.decodeCoolantTemp(vec);
                        ui->coolantLcd->display(temp);
                        ui->coolant_dial->setValue(temp);
                        break;
                    }
                    case 12:{
                        //rpm;
                        int rpm = mySharedData->dataParser.decodeEngineRPM(vec);
                        ui->rpmLcd->display(rpm);
                        ui->rpm_dial->setValue(rpm);
                        break;
                    }
                    case 13 :{
                        //speed
                        int speed = mySharedData->dataParser.decodeKmHSpeed(vec);
                        ui->speedLcd->display(speed);
                        ui->speed_dial->setValue(speed);

                        break;
                    }
                    case 15 : {
                        //intake temp;
                        int intake_temp = mySharedData->dataParser.decodeIntakeAirTemp(vec);
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
            resp= mySharedData->dataParser.prepareResponseToDecode(response);
            if(!resp[0].compare(QString("NO DATA"),Qt::CaseInsensitive)){
                return;
            }
            //number of dtc & mil
            if(resp.size()>2 && !resp[2].compare("41",Qt::CaseInsensitive) && !resp[3].compare("01",Qt::CaseInsensitive)){
                vec.insert(vec.begin(),resp.begin()+4, resp.end());
                std::pair<int,bool> dtcNumber = mySharedData->dataParser.decodeNumberOfDtc(vec);
                ui->dtc_numberEdit->setText(QString::number(dtcNumber.first));
                ui->dtc_milIndicatorON->setChecked(dtcNumber.second);
            }
            //dtc codes
            if(resp.size()>2 && !resp[1].compare("43",Qt::CaseInsensitive)){
               vec.insert(vec.begin(),resp.begin()+1, resp.end());
               std::vector<QString> dtcCodes( mySharedData->dataParser.decodeDTC(vec));
               ui->dtc_descriptionEdit->append(QString("-------- RAPORT --------"));
               ui->dtc_descriptionEdit->append(QString("-----------------------------"));

               ui->dtc_descriptionEdit->append(QString((QDateTime::currentDateTime()).toString(QString("yyyy-MM-dd hh:mm:ss"))));
               ui->dtc_descriptionEdit->append(QString("-----------------------------"));
               if(dtcCodes.size()>0){
                   for(auto &code : dtcCodes){

                       ui->dtc_descriptionEdit->append(code);
                       ui->dtc_descriptionEdit->append(QString("-----------------------------"));
                   }
               }
               ui->dtc_descriptionEdit->append(QString("---------- END ---------"));

            }
            //clear DTCs
            if(resp.size()>2 && !resp[1].compare("44",Qt::CaseInsensitive)){
                ui->dtc_confirmatioFrame->setEnabled(false);
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
    mySharedData->log->logDebbug("starting RTD reading");
    if(mySharedData->dataExchanger==nullptr)
        return;
    mySharedData->dataExchanger->setContinueRequesting(true);
    mySharedData->dataExchanger->sendDataToElm327(QString("01 0F"));
    mySharedData->dataExchanger->sendDataToElm327("01 0D");
    mySharedData->dataExchanger->sendDataToElm327(QString("01 0C"));
    mySharedData->dataExchanger->sendDataToElm327(QString("01 05"));

  //  }
}

void OBDScanner::on_dtc_checkErrNumberButton_clicked()
{
    if(activeTab==-1)
        activeTab=1;
    ui->dtc_numberEdit->clear();
    ui->dtc_milIndicatorON->setChecked(false);
    if(mySharedData->dataExchanger!=nullptr)
        mySharedData->dataExchanger->sendDataToElm327("01 01");
}

void OBDScanner::on_dtc_getErrCodesButton_clicked()
{
    if(activeTab==-1)
        activeTab=1;
    ui->dtc_descriptionEdit->setText(QString(""));
    if(mySharedData->dataExchanger!=nullptr)
        mySharedData->dataExchanger->sendDataToElm327(QString("03"));

}

void OBDScanner::on_dtc_clearErrors_clicked()
{
    if(activeTab==-1)
        activeTab=1;
    ui->dtc_confirmatioFrame->setEnabled(true);
    ui->dtc_confirmationBox->setEnabled(true);
}

void OBDScanner::on_dtc_confirmationBox_accepted()
{
    mySharedData->log->logDebbug("usuwamy błędy!!!");
    ui->dtc_confirmationBox->setEnabled(false);
    if(mySharedData->dataExchanger!=nullptr){
        mySharedData->dataExchanger->sendDataToElm327("04");

    }
}

void OBDScanner::on_dtc_confirmationBox_rejected()
{
    ui->dtc_confirmatioFrame->setEnabled(false);
}

void OBDScanner::on_rtd_startButton_clicked()
{
    if(activeTab==-1)
        activeTab=0;
    getRTD();
}
