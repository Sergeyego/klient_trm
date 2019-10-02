#-------------------------------------------------
#
# Project created by QtCreator 2011-06-20T12:27:29
#
#-------------------------------------------------

QT       += core gui sql serialbus

TARGET = klient_trm
TEMPLATE = app

CONFIG += qwt

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += main.cpp\
    mainwidget.cpp \
    cfgplot.cpp \
    oven.cpp \
    prog.cpp \
    inputdialog.cpp \
    modelchannel.cpp \
    tablemodel.cpp \
    sqlengine.cpp \
    widgets.cpp \
    modbusthread.cpp \
    delegate.cpp

HEADERS  += \
    mainwidget.h \
    cfgplot.h \
    oven.h \
    prog.h \
    inputdialog.h \
    modelchannel.h \
    tablemodel.h \
    sqlengine.h \
    widgets.h \
    modbusthread.h \
    delegate.h

FORMS    += \
    mainwidget.ui \
    cfgplot.ui \
    prog.ui \
    oven.ui \
    inputdialog.ui

LIBS += -lqwt-qt5
INCLUDEPATH += /usr/include/qt5/qwt

RESOURCES += \
    res.qrc
