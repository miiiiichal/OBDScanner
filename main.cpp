#include "obdscanner.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  OBDScanner w;
  w.show();

  return a.exec();
}
