#include "btconnector.h"
#include "ui_btconnector.h"

BtConnector::BtConnector(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::BtConnector)
{
  ui->setupUi(this);
}

BtConnector::~BtConnector()
{
  delete ui;
}
