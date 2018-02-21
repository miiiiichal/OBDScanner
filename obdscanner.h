#ifndef OBDSCANNER_H
#define OBDSCANNER_H

#include <QMainWindow>
#include "btconnector.h"
#include "datakeeper.h"
#include "obddataparser.h"
#include <memory>
#include <string>



namespace Ui {
  class OBDScanner;
}


class OBDScanner : public QMainWindow
{
  Q_OBJECT

public:
    enum ActiveTab {
        Info=-1,
        Rtd,
        Dtc,
        Cmd
    };

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

  //void on_pushButton_clicked();

  void on_dtc_checkErrNumberButton_clicked();

  void on_dtc_getErrCodesButton_clicked();

  void on_dtc_clearErrors_clicked();

  void on_dtc_confirmationBox_accepted();

  void on_dtc_confirmationBox_rejected();

  void on_rtd_startButton_clicked();

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
