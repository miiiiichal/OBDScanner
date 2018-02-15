#ifndef DATAKEEPER_H
#define DATAKEEPER_H

#include <QBluetoothLocalDevice>
#include <QBluetoothSocket>
#include "logger.h"
#include "obddataparser.h"
#include "obddataexchanger.h"

class DataKeeper
{
public:
    DataKeeper();
    ~DataKeeper();
    DataKeeper( const DataKeeper &);
    DataKeeper& operator=(const DataKeeper &);
    DataKeeper(DataKeeper *);

    QBluetoothLocalDevice *localDevice = nullptr;
    ObdDataExchanger *dataExchanger = nullptr;
    Logger *log = nullptr;
    QBluetoothSocket *mySocket = nullptr;
    ObdDataParser dataParser;

    QString test ="aaaaa";

};

#endif // DATAKEEPER_H
