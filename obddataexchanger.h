#ifndef OBDDATAEXCHANGER_H
#define OBDDATAEXCHANGER_H

#include <QBluetoothSocket>
#include "logger.h"
#include <QObject>

class ObdDataExchanger : public QObject
{
    Q_OBJECT
private:
    QBluetoothSocket *mySocket;
    Logger *log;

    QVector<QString> responseRegister;
    QByteArray lastResponse;
public:
    explicit ObdDataExchanger(QObject *parent = nullptr);
    ObdDataExchanger(QBluetoothSocket * ,Logger *);

    void setSocket(QBluetoothSocket *);
    void setLogger(Logger *);
    QBluetoothSocket* getSocket();

   // void sendDataToElm327(QByteArray);
    void sendDataToElm327(QString &);
    QString getLastResponse();

signals:
    void readDataReady();
    void readDataReady(QString);

public slots:
    void getDataFromElm327();
    void readingError(QBluetoothSocket::SocketError);

};

#endif // OBDDATAEXCHANGER_H
