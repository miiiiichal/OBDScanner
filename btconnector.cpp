#include "btconnector.h"
#include "ui_btconnector.h"

BtConnector::BtConnector(QBluetoothLocalDevice &localDev, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::BtConnector)
{
  ui->setupUi(this);
  localDevice = &localDev;
  QBluetoothAddress localDevAddr = localDevice->address();

  ui->lineEdit_address->setText(localDevAddr.toString());
  ui->lineEdit_name->setText(localDevice->name());
  setWindowTitle("Bluetooth device management");
}

BtConnector::~BtConnector()
{
  delete ui;
}


