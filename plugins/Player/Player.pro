#-------------------------------------------------
#
# Project created by QtCreator 2016-04-24T17:59:02
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = Player
TEMPLATE = lib
CONFIG  += plugin


SOURCES +=  player.cpp \
         ../../app/nomain/rdata.cpp


HEADERS  += player.h \
         ../../app/typelist.h \
         ../../app/pluginterface.h \
         ../../app/nomain/rdata.h


FORMS    += player.ui

DISTFILES += Player.json

DESTDIR         = ../../plugin

EXAMPLE_FILES = Player.json


target.path = ../../plugin
INSTALLS += target

