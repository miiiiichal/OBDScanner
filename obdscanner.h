#ifndef OBDSCANNER_H
#define OBDSCANNER_H

#include <QMainWindow>
#include "btconnector.h"
#include "datakeeper.h"
#include <memory>



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

//TEST
    struct myData2{
        std::shared_ptr<QBluetoothSocket> socketPtr;
        QBluetoothLocalDevice localDevice;
        ObdDataExchanger *dataExchanger;
        Logger *log;
        QBluetoothSocket *mySocket;
    }  ;

  explicit OBDScanner(QWidget *parent = 0);
  ~OBDScanner();

public slots:
    void getSocketFromConnector(QBluetoothSocket*,QString);
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
  void getRTD();

private:
  Ui::OBDScanner *ui;
  QBluetoothLocalDevice localDevice;
  Logger *log;
  ObdDataExchanger *dataExchanger;

 // DataKeeper myData;

  std::shared_ptr<DataKeeper> mySharedData;



  //--
  int activeTab=ActiveTab::Info;

};

#endif // OBDSCANNER_H
