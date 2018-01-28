#ifndef OBDDATAPARSER_H
#define OBDDATAPARSER_H

#include <QObject>

class ObdDataParser
{
public:
    ObdDataParser();

    QString decodeDTC(const QString &);
    QString getDTCDescription(const QString &);
    int decodeKmHSpeed(const QString &);
    int decodeEngineRPM(const QString &);
    int decodeCoolantTemp(const QString &);
    int decodeVehicleIdNumber(const std::vector<QString> &);
};
#endif
