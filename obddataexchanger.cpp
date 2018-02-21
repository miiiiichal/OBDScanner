#include "obddataexchanger.h"

ObdDataExchanger::ObdDataExchanger(QObject *parent) : QObject(parent)
{

}

ObdDataExchanger::ObdDataExchanger(QBluetoothSocket *socket, Logger *logger) : mySocket(socket), log(logger)
{
    log->logDebbug("DataExchanger Created");
    connect(mySocket, SIGNAL(readyRead()), this, SLOT(getDataFromElm327()));
    connect(mySocket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(readingError(QBluetoothSocket::SocketError)));
    connect(this,SIGNAL(sendNextRequest()),this,SLOT(writeToSocket()));
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
        requestRegister.push(instruction);
        log->logDebbug("Add next instr to queue");
        emit(sendNextRequest());
    }
}

void ObdDataExchanger::writeToSocket(){
    if(!requestRegister.empty() && lockSocket==false){
        QString instruction = requestRegister.front();
        requestRegister.pop();
        if(continueRequesting){
            requestRegister.push(instruction);
        }
        QByteArray buffer(instruction.toStdString().c_str());
        if(mySocket!=nullptr){
            mySocket->write(buffer);
            lockSocket=true;
            log->logDebbug("writing TO SOCKET");
            log->logInfo("write instr: "+instruction);
            //a w tym miejscu założony semafor
        }
    }
}

void ObdDataExchanger::getDataFromElm327(){
    if(mySocket->isReadable()){
        if(mySocket->bytesAvailable()>0){
           QByteArray line = mySocket->readLine();
           log->logDebbug("Reading FROM SOCKET : " + line);
           lastResponse.append(line);
           if(line.contains("\r\r") || line.contains(">")){
                lockSocket=false;
                responseRegister.push_back(QString(lastResponse));
                log->logInfo("Read FROM SOCKET : whole response : " + lastResponse);
                emit readDataReady(getLastResponse());
                emit sendNextRequest();
                lastResponse.clear();

           }
        }
    }
    else{
        log->logInfo("Cannot read from socket" );
        log->logDebbug("isRedable() false ");
    }
}


QString ObdDataExchanger::getLastResponse()
{
    if(responseRegister.isEmpty())
        return QString("");
    else
        return QString(responseRegister.takeLast());
}

void ObdDataExchanger::setContinueRequesting(bool t_f_val)
{
    continueRequesting=t_f_val;
}

void ObdDataExchanger::clearRegisters()
{
    log->logDebbug("CLEARING REGISTERS");
    if(!responseRegister.empty())
        responseRegister.clear();

    log->logDebbug(QString("responseREg cleared: "));
    if(!requestRegister.empty()){
        std::queue<QString> emptyQueue;
        std::swap(requestRegister,emptyQueue);
    }
    log->logDebbug(QString("requestREg cleared: "));
    lockSocket = false;

}

bool ObdDataExchanger::getContinueRequesting()
{
    return continueRequesting;
}

void ObdDataExchanger::readingError(QBluetoothSocket::SocketError)
{
    log->logError("err: " + mySocket->errorString());

}



