#ifndef OBDDATAPARSER_H
#define OBDDATAPARSER_H

#include <QObject>
#include <vector>

class ObdDataParser
{
public:
    ObdDataParser();

    QString decodeDTC(const std::vector<QString>&);
    QString getDTCDescription(const std::vector<QString> &);

    int decodeKmHSpeed(const std::vector<QString> &);
    int decodeEngineRPM(const std::vector<QString> &);
    int decodeCoolantTemp(const std::vector<QString> &);
    int decodeIntakeAirTemp(const std::vector<QString> &);
    int decodeVehicleIdNumber(const std::vector<QString> &);

    std::vector<QString> prepareResponseToDecode(const QString &);



};
#endif
