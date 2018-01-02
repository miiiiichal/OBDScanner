#ifndef OBDSCANNER_H
#define OBDSCANNER_H

#include <QMainWindow>

namespace Ui {
  class OBDScanner;
}

class OBDScanner : public QMainWindow
{
  Q_OBJECT

public:
  explicit OBDScanner(QWidget *parent = 0);
  ~OBDScanner();

private:
  Ui::OBDScanner *ui;
};

#endif // OBDSCANNER_H
