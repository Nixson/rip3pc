#ifndef PLUGINTERFACE_H
#define PLUGINTERFACE_H

#include <QObject>
#include <QWidget>
#include "nomain/rdata.h"

class SecondaryProcessingInterfaces: public QObject
{
public:
    virtual ~SecondaryProcessingInterfaces() {}
signals:
    void processComplete();
};

#define SecondaryProcessingInterfaces_iid "ru.nixson.physicks.Rip3p.SecondaryProcessingInterfaces"

Q_DECLARE_INTERFACE(SecondaryProcessingInterfaces, SecondaryProcessingInterfaces_iid);


class PlugWidgetInterfaces: public QWidget
{
public:
    virtual ~PlugWidgetInterfaces() {}
    virtual void setMemory(Rdata *mLink) = 0;
    virtual void init() = 0;
signals:
    void processStep(int);
    void processStop(int);
    void processStart();
    void sync();

public slots:
    void syncSlot();
    /*void setMemory(Rdata *mLink);
    void init();*/
};

#define PlugWidgetInterfaces_iid "ru.nixson.physicks.Rip3p.PlugWidgetInterfaces"

Q_DECLARE_INTERFACE(PlugWidgetInterfaces, PlugWidgetInterfaces_iid);






#endif // PLUGINTERFACE_H
