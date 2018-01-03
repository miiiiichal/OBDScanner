#include "obdscanner.h"
#include "ui_obdscanner.h"

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
