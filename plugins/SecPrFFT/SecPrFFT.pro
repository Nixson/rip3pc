#-------------------------------------------------
#
# Project created by QtCreator 2016-04-12T09:37:15
#
#-------------------------------------------------

QT       += core gui

QT += widgets

TARGET = SecPrFFT
TEMPLATE = lib
CONFIG  += plugin

SOURCES += secondaryprocessing.cpp

HEADERS  += secondaryprocessing.h\
         ../../app/pluginterface.h


DESTDIR         = ../../plugin

EXAMPLE_FILES = SecondaryProcessing.json


target.path = ../../plugin
INSTALLS += target
