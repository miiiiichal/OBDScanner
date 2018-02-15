#include "datakeeper.h"

DataKeeper::DataKeeper()
{
localDevice=new QBluetoothLocalDevice();
}

DataKeeper::~DataKeeper()
{
delete localDevice;
}


DataKeeper::DataKeeper( const DataKeeper &other)
{
    localDevice =other.localDevice;
    dataExchanger = other.dataExchanger;
    log = other.log;
    mySocket = other.mySocket;
    dataParser= other.dataParser;

}

DataKeeper::DataKeeper(DataKeeper *other)
{
    if(this!=other){
        this->localDevice =other->localDevice;
        this->dataExchanger = other->dataExchanger;
        this->log = other->log;
        this->mySocket = other->mySocket;
        this->dataParser=other->dataParser;
    }
}

DataKeeper& DataKeeper::operator=(const DataKeeper &other){
    this->localDevice=other.localDevice;
    this->dataExchanger = other.dataExchanger;
    this->log = other.log;
    this->mySocket = other.mySocket;
    this->dataParser = other.dataParser;

    return *this;
}
