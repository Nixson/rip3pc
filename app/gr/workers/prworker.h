#ifndef PRWORKER_H
#define PRWORKER_H

#include <QObject>
#include "typelist.h"
#include "qcustomplot.h"


class PRworker : public QObject
{
    Q_OBJECT
public:
    explicit PRworker(QObject *parent = 0);

signals:
    void plot(QCPColorMapData *data);

public slots:
    void syncSlot(QString type);
    void dataSlot(Clowd &dataA, Clowd &dataH);
    void plotSlot();
    void sharedSlot(int ship, QString type);
private:
    int m_count, AngleMin, AngleMax, OffsetMin, OffsetMax, Size, maxColor, leRasterPeriod, Lay;
    uint MaxBarier;
    Clowd m_data, histA, histY;
    QCPColorMap *colorMap;
    bool rbAmp, rbPh;
};

#endif // PRWORKER_H
