#ifndef PLOTRASTER_H
#define PLOTRASTER_H

#include <QWidget>
#include <QTimer>
#include "qcustomplot.h"
#include "typelist.h"
#include "workers/prworker.h"






namespace Ui {
class PlotRaster;
}

class PlotRaster : public QWidget
{
    Q_OBJECT
    QThread mathThread;

public:
    explicit PlotRaster(QWidget *parent = 0);
    ~PlotRaster();
    void setType(QString type);
    void setBuf(Clowd &Ar, Clowd &Ph);
    void shared(int);

signals:
    void sync();
    void dataSignal(Clowd &Ar, Clowd &Ph);
    void syncSignal(QString type);
    void plotSignal();
    void sharedSignal(int ship, QString type);

public slots:
    void syncSlot();
    void plot();
    void result(QCPColorMapData *data);
    void sharedNow();

private slots:
    void on_rbAmp_toggled(bool checked);

    void on_rbPh_toggled(bool checked);

private:
    Ui::PlotRaster *ui;
    QCustomPlot *customPlot;
    QCPColorMap *colorMap;
    int Size, Barier, ArgMin, ArgMax, PhMin, PhMax, sharedShip;
    int leRasterPeriod, Lay;
    QString Type;
    Clowd Amp, Phase;
    PRworker *worker;
    QTimer *sharedTimer;
    bool isShared;
};

#endif // PLOTRASTER_H
