#ifndef OBDSCANNER_H
#define OBDSCANNER_H

#include <QMainWindow>
#include <QBluetoothLocalDevice>
#include <QBluetoothSocket>
#include "logger.h"
#include "btconnector.h"





namespace Ui {
  class OBDScanner;
}

class OBDScanner : public QMainWindow
{
  Q_OBJECT

public:
  explicit OBDScanner(QWidget *parent = 0);
  ~OBDScanner();

public slots:
    void getSignalFromConnector(QBluetoothSocket*);

private slots:
  void on_btRadioButton_clicked(bool checked);
  void on_btConfigButton_clicked();


private:
  Ui::OBDScanner *ui;
  QBluetoothLocalDevice localDevice;
  Logger *log;

};

#endif // OBDSCANNER_H
