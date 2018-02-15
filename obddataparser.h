#ifndef OBDDATAPARSER_H
#define OBDDATAPARSER_H

#include <QObject>
#include <vector>
#include <map>

class ObdDataParser
{
public:
    ObdDataParser();

    std::vector<QString> decodeDTC(const std::vector<QString>&);
    QString getDTCDescription(const std::vector<QString> &);

    int decodeKmHSpeed(const std::vector<QString> &);
    int decodeEngineRPM(const std::vector<QString> &);
    int decodeCoolantTemp(const std::vector<QString> &);
    int decodeIntakeAirTemp(const std::vector<QString> &);
    int decodeVehicleIdNumber(const std::vector<QString> &);
    std::pair<int,bool>  decodeNumberOfDtc(const std::vector<QString> &);
    std::vector<QString> prepareResponseToDecode(const QString &);

private:
    std::map<char,QString> dtcPrefix={{'0',QString("P0")},{'1',QString("P1")},{'2',QString("P2")},{'3',QString("P3")},
                                      {'4',QString("C0")},{'5',QString("C1")},{'6',QString("C2")},{'7',QString("C3")},
                                      {'8',QString("B0")},{'9',QString("B1")},{'A',QString("B2")},{'B',QString("B3")},
                                      {'C',QString("U0")},{'D',QString("U1")},{'E',QString("U2")},{'F',QString("U3")}
                                     };

};
#endif
