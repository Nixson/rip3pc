#-------------------------------------------------
#
# Project created by QtCreator 2016-04-09T19:44:10
#
#-------------------------------------------------

QT       += core gui widgets multimedia printsupport network

TARGET = rip3p
TEMPLATE = app


SOURCES += memory.cpp\
        main.cpp\
        gwindow.cpp \
    qcustomplot.cpp \
    gr/gr3d.cpp \
    mcontrol.cpp \
    nomain/scobject.cpp \
    gr/plotgl.cpp \
    plugwin.cpp \
    nomain/udpsock.cpp \
    nomain/formsettings.cpp \
    worker.cpp \
    nomain/debugdialog.cpp \
    nomain/extparam.cpp \
    nomain/rdata.cpp \
    gr/plotpolarization.cpp \
    gr/workers/scworker.cpp \
    gr/workers/ppworker.cpp \
    gr/plotraster.cpp \
    gr/workers/prworker.cpp \
    nomain/savefile.cpp \
    nomain/controlform.cpp \
    gr/plotosc.cpp

HEADERS  += memory.h \
    gwindow.h \
    qcustomplot.h \
    gr/gr3d.h \
    mcontrol.h \
    typelist.h \
    nomain/scobject.h \
    gr/plotgl.h \
    plugwin.h \
    pluginterface.h \
    nomain/udpsock.h \
    nomain/formsettings.h \
    worker.h \
    nomain/debugdialog.h \
    nomain/extparam.h \
    nomain/rdata.h \
    gr/plotpolarization.h \
    gr/workers/scworker.h \
    gr/workers/ppworker.h \
    gr/plotraster.h \
    gr/workers/prworker.h \
    nomain/savefile.h \
    nomain/controlform.h \
    gr/plotosc.h

FORMS    += gwindow.ui \
    gr/gr3d.ui \
    plugwin.ui \
    nomain/formsettings.ui \
    nomain/debugdialog.ui \
    nomain/extparam.ui \
    gr/plotpolarization.ui \
    gr/plotraster.ui \
    gr/plotmath.ui \
    nomain/savefile.ui \
    nomain/controlform.ui \
    gr/plotosc.ui

RESOURCES += \
    nomain/res.qrc

target.path = ..
INSTALLS += target

DESTDIR = ..

QMAKE_CXXFLAGS += -std=c++11
#LIBS    += -Wl,-rpath,../qpcap -L../qpcap -lqpcap
#INCLUDEPATH += ../qpcap
#QMAKE_RPATHDIR += ../qpcap
LIBS += -lpcap
