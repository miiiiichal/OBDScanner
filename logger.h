#ifndef LOGGER_H
#define LOGGER_H

#include <QFile>
#include <QFileDevice>
#include <QTextStream>
#include <QDateTime>



class Logger
{

public:
  enum LogLevel {
      Error=1,
      Info,
      Debbug
  };


  Logger();
  Logger(QString);
//  Logger(QString, Logger::LogLevel);
  ~Logger();


  LogLevel getLogLevel();
  void setLogLevel(Logger::LogLevel level);
  void logError(QString);
  void logInfo(QString);
  void logDebbug(QString);


private:
QFile *logFile = nullptr;
QString filePath = "OBDScannerLog.txt";
LogLevel logLevel = LogLevel::Debbug;
void writeToLog(QString);

};

#endif // LOGGER_H
