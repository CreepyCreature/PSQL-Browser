#-------------------------------------------------
#
# Project created by QtCreator 2017-01-29T14:48:07
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PSQL-Browser
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    connectiondialog.cpp \
    tableschema.cpp \
    columnschema.cpp

HEADERS  += mainwindow.h \
    connectiondialog.h \
    tableschema.h \
    columnschema.h

FORMS    += mainwindow.ui \
    connectiondialog.ui
