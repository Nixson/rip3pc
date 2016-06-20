#ifndef SECONDARYPROCESSING_H
#define SECONDARYPROCESSING_H

#include <QObject>
#include <QtPlugin>
#include "../../app/pluginterface.h"

class SecondaryProcessing : public SecondaryProcessingInterfaces
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "ru.nixson.physicks.Rip3p.SecondaryProcessingInterfaces" FILE "SecondaryProcessing.json")
    Q_INTERFACES(SecondaryProcessingInterfaces)

public:
    explicit SecondaryProcessing();
    ~SecondaryProcessing(){}
signals:

public slots:
};

#endif // SECONDARYPROCESSING_H
