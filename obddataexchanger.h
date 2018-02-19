#ifndef OBDDATAEXCHANGER_H
#define OBDDATAEXCHANGER_H

#include <QBluetoothSocket>
#include "logger.h"
#include <memory>
#include <QObject>
#include <queue>


class ObdDataExchanger : public QObject
{
    Q_OBJECT
private:
    QBluetoothSocket *mySocket;
    Logger *log;

    QVector<QString> responseRegister;
    std::queue<QString> requestRegister;
    QByteArray lastResponse;
    bool lockSocket=false;
    bool continueRequesting=false;

public:
    explicit ObdDataExchanger(QObject *parent = nullptr);
    ObdDataExchanger(QBluetoothSocket * ,Logger *);
  //  ObdDataExchanger(std::shared_ptr<DataKeeper> &);

    void setSocket(QBluetoothSocket *);
    void setLogger(Logger *);
    QBluetoothSocket* getSocket();

   // void sendDataToElm327(QByteArray);
    void sendDataToElm327(const QString &);
    void sendDataToElm327(const char*);
    QString getLastResponse();
    void setContinueRequesting(bool);
    bool getContinueRequesting();
    void clearRegisters();

signals:
    void readDataReady();
    void readDataReady(QString);
    void sendNextRequest();

public slots:
    void getDataFromElm327();
    void readingError(QBluetoothSocket::SocketError);
  //  void writeToSocket(QString);
    void writeToSocket();


};

#endif // OBDDATAEXCHANGER_H
