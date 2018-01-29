#ifndef OBDSCANNER_H
#define OBDSCANNER_H

#include <QMainWindow>
#include <QBluetoothLocalDevice>
#include <QBluetoothSocket>
#include "logger.h"
#include "btconnector.h"
#include "obddataexchanger.h"





namespace Ui {
  class OBDScanner;
}

class OBDScanner : public QMainWindow
{
  Q_OBJECT

public:
    enum ActiveTab {
        Info=0,
        Rtd,
        Dtc,
        Cmd
    };

  explicit OBDScanner(QWidget *parent = 0);
  ~OBDScanner();

public slots:
    void getSocketFromConnector(QBluetoothSocket*);
    void getDeviceInfoFromConnector(QBluetoothDeviceInfo*);
    void getSignalFromConnector(QString );

private slots:
  void on_btRadioButton_clicked(bool checked);
  void on_btConfigButton_clicked();
  void on_tabWidget_tabBarClicked(int index);
  void on_cmd_clearButton_clicked();
  void on_cmd_sendButton_clicked();

  void obdResponseDispatcher(QString);
  void obdResponseDispatcher();

private:
  Ui::OBDScanner *ui;
  QBluetoothLocalDevice localDevice;
  Logger *log;
  //BtConnector *btConnectorForm;
  ObdDataExchanger *dataExchanger;


  //--
  int activeTab=ActiveTab::Info;

};

#endif // OBDSCANNER_H
