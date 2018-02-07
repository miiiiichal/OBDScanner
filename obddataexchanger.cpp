#include "obddataexchanger.h"

ObdDataExchanger::ObdDataExchanger(QObject *parent) : QObject(parent)
{

}

ObdDataExchanger::ObdDataExchanger(QBluetoothSocket *socket, Logger *logger) : mySocket(socket), log(logger)
{
    log->logDebbug("DataExchanger Created");
    connect(mySocket, SIGNAL(readyRead()), this, SLOT(getDataFromElm327()));
    connect(mySocket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(readingError(QBluetoothSocket::SocketError)));
}

//ObdDataExchanger::ObdDataExchanger(std::shared_ptr<DataKeeper> &sharedData)
//{
//    log=sharedData->log;
//    mySocket=sharedData->mySocket;
//}

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

void ObdDataExchanger::sendDataToElm327(const char* str){
    QString instr(str);
    sendDataToElm327(instr);
}

void ObdDataExchanger::sendDataToElm327(const QString &instr)
{
    QString instruction(instr);
    if(!instruction.isEmpty()){
        instruction.append("\r");
        QByteArray buffer(instruction.toStdString().c_str());
        if(mySocket!=nullptr){
            mySocket->write(buffer);
            log->logInfo("write instr: "+instruction);
            //a w tym miejscu założony semafor
        }
    }
}

QString ObdDataExchanger::getLastResponse()
{
    if(responseRegister.isEmpty())
        return QString("");
    else
        return QString(responseRegister.takeFirst());
}

void ObdDataExchanger::getDataFromElm327(){
    if(mySocket->isReadable()){
        if(mySocket->bytesAvailable()>0){
           QByteArray line = mySocket->readLine();
           log->logDebbug("Reading FROM SOCKET : " + line);
           lastResponse.append(line);
           if(line.contains("\r\r")){
                responseRegister.push_back(QString(lastResponse));
                log->logInfo("Read FROM SOCKET : whole response : " + lastResponse);
                //w tym miejscu powinien być zwolniony semafor?? lockGuard??
                emit readDataReady(getLastResponse());
                lastResponse.clear();
           }
        }
    }
    else{
        log->logInfo("Cannot read from socket" );
        log->logDebbug("isRedable() false ");
    }
}

void ObdDataExchanger::readingError(QBluetoothSocket::SocketError)
{
    log->logError("err: " + mySocket->errorString());

}



