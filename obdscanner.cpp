#include "obdscanner.h"
#include "ui_obdscanner.h"
#include "btconnector.h"

OBDScanner::OBDScanner(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::OBDScanner)
{
  ui->setupUi(this);
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
}
