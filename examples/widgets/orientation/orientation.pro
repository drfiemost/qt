#-------------------------------------------------
#
# Project created by QtCreator 2010-08-04T10:27:31
#
#-------------------------------------------------

QT       += core gui

TARGET = orientation
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS += \
    portrait.ui \
    landscape.ui

RESOURCES += \
    images.qrc


simulator: warning(This example might not fully work on Simulator platform)
