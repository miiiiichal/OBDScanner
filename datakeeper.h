#ifndef DATAKEEPER_H
#define DATAKEEPER_H

#include <QBluetoothLocalDevice>
#include <QBluetoothSocket>
#include "logger.h"

#include "obddataexchanger.h"

class DataKeeper
{
public:
    DataKeeper();
    ~DataKeeper();
    DataKeeper( const DataKeeper &);
    DataKeeper& operator=(const DataKeeper &);
    DataKeeper(DataKeeper *);

    QBluetoothLocalDevice *localDevice;
    ObdDataExchanger *dataExchanger;
    Logger *log;
    QBluetoothSocket *mySocket;
    QString test ="aaaaa";
};

#endif // DATAKEEPER_H
