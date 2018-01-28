#include "obddataexchanger.h"

ObdDataExchanger::ObdDataExchanger(QObject *parent) : QObject(parent)
{

}

ObdDataExchanger::ObdDataExchanger(QBluetoothSocket *socket, Logger *logger) : mySocket(socket), log(logger)
{
    log->logDebbug("DataExchanger Created");

}

void ObdDataExchanger::setSocket(QBluetoothSocket *socket)
{
    if(socket->isOpen())
        mySocket=socket;
    else
        mySocket=nullptr;
}

void ObdDataExchanger::setLogger(Logger *logger)
{
    if(log!=logger)
        log=logger;
}

QBluetoothSocket* ObdDataExchanger::getSocket(){
    return(mySocket);
}



void ObdDataExchanger::sendDataToElm327(QString &instruction )
{
    data.clear();
    if(!instruction.isEmpty()){
        instruction.append("\r");
        QByteArray buffer(instruction.toStdString().c_str());
        if(mySocket->isWritable()){
            mySocket->write(buffer);
            log->logInfo("write instr: "+instruction);
        }
    }
}

QVector<QByteArray> ObdDataExchanger::getData()
{
    return data;
}

void ObdDataExchanger::getDataFromElm327(){
    QString readData;

    if(mySocket->isReadable()){
        if(mySocket->bytesAvailable()>0){
           QByteArray line = mySocket->readLine().trimmed();
           log->logInfo("Read FROM SOCKET : raw " + line);
           data.push_back(line);
        }
    }
    else{
        log->logInfo("Cannot read from socket" );
        log->logDebbug("isRedable() false ");
    }
}

void ObdDataExchanger::readingError(QBluetoothSocket::SocketError)
{
    //debugInfo("err: " + mySocket->errorString());
    log->logDebbug("err: " + mySocket->errorString());
}



