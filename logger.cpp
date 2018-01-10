#include "logger.h"



Logger::Logger()
{
  filePath = "./OBDScannerLog.txt";
  logFile = new QFile(filePath);
  logFile->open(QFile::WriteOnly);
}

Logger::Logger(QString fileName) : filePath(fileName){
  logFile = new QFile(fileName);
  if(logFile->exists()){
    QDateTime dateTime;
    QString oldLogName((dateTime.currentDateTime()).toString("yyyyMMddhhmmss"));
    oldLogName+="_"+logFile->fileName();
    if(logFile->copy(oldLogName)){
        logFile->flush();
      }
  }
  logFile->open(QFile::WriteOnly);
}
//not working
//Logger::Logger(QString fileName, Logger::LogLevel level) : filePath(fileName), logLevel(level) {}

Logger::~Logger()
{
 if(logFile->isOpen())
    logFile->close();
 delete logFile;
}

void Logger::writeToLog(QString textToWrite)
{
  if(!logFile->isOpen())
    logFile->open(QFile::WriteOnly);
  if(logFile->isWritable()){
      QTextStream out(logFile);
      out<<textToWrite+"\n";
  }
}
void Logger::logError(QString errorMsg)
{
   writeToLog("Error: "+errorMsg);
}

void Logger::logInfo(QString infoMsg)
{
  if(logLevel==Logger::Info)
    writeToLog("Info: "+infoMsg);
  }
void Logger::logDebbug(QString debbugMsg)
{
  if(logLevel==Logger::Debbug)
    writeToLog("Debbug: "+debbugMsg);
}

Logger::LogLevel Logger::getLogLevel(){
  return logLevel;
}
void Logger::setLogLevel(Logger::LogLevel level){
  logLevel=level;
}
