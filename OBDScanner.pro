#-------------------------------------------------
#
# Project created by QtCreator 2018-01-02T22:45:26
#
#-------------------------------------------------

QT       += core gui bluetooth

# With C++11 support
greaterThan(QT_MAJOR_VERSION, 4){
CONFIG += c++11
} else {
QMAKE_CXXFLAGS += -std=c++0x
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OBDScanner
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        obdscanner.cpp \
    btconnector.cpp \
    logger.cpp \
    obddataexchanger.cpp \
    obddataparser.cpp

HEADERS += \
        obdscanner.h \
    btconnector.h \
    logger.h \
    obddataexchanger.h \
    obddataparser.h

FORMS += \
        obdscanner.ui \
    btconnector.ui
