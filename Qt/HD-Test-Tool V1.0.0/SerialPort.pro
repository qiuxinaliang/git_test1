#**********************************************************
#Author: Qt君
#微信公众号: Qt君
#QQ群: 732271126
#LICENSE: MIT
#**********************************************************
QT       += core gui
QT       += serialport
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = SerialPort
TEMPLATE = app

win32: RC_ICONS += app.ico

SOURCES += main.cpp\
    serial/mainwindow.cpp \
    serial/qcustomplot.cpp \
    anchor/anchor_para_set.cpp \
    rtls_postion/rtlc_posview.cpp \
    network/udpsever.cpp \
    public/public.cpp

HEADERS  += serial/mainwindow.h \
    qcustomplot.h \
    anchor/anchor_para_set.h \
    rtls_postion/rtls_posview.h \
    network/udpsever.h \
    public/public.h

FORMS    += serial/mainwindow.ui
