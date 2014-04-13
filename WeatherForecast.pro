#-------------------------------------------------
#
# Project created by QtCreator 2014-03-16T14:09:14
#
#-------------------------------------------------

QT       += core gui network xml

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WeatherForecast
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    downloadmanager.cpp \
    xmlparser.cpp

HEADERS  += mainwindow.h \
    downloadmanager.h \
    xmlparser.h

FORMS    += mainwindow.ui

RESOURCES += \
    MyResource.qrc
