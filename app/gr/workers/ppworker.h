#ifndef PPWORKER_H
#define PPWORKER_H

#include <QObject>
#include "typelist.h"
#include "qcustomplot.h"


class PPworker : public QObject
{
    Q_OBJECT
public:
    explicit PPworker(QObject *parent = 0);

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

};

#endif // PPWORKER_H
