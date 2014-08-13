#-------------------------------------------------
#
# Project created by QtCreator 2014-07-09T18:50:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BitmapGlue
TEMPLATE = app


SOURCES += main.cpp\
        bitmapglue.cpp \
    helpwindow.cpp

HEADERS  += bitmapglue.hpp \
    helpwindow.hpp

FORMS    += bitmapglue.ui \
    helpwindow.ui

RESOURCES += \
    resources.qrc

RC_FILE += \
    icon.rc
