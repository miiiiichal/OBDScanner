#ifndef OBDSCANNER_H
#define OBDSCANNER_H

#include <QMainWindow>
#include <QBluetoothLocalDevice>




namespace Ui {
  class OBDScanner;
}

class OBDScanner : public QMainWindow
{
  Q_OBJECT

public:
  explicit OBDScanner(QWidget *parent = 0);
  ~OBDScanner();

private slots:
  void on_btRadioButton_clicked(bool checked);

  void on_btConfigButton_clicked();

private:
  Ui::OBDScanner *ui;
  QBluetoothLocalDevice localDevice;

};

#endif // OBDSCANNER_H
