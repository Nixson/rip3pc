#ifndef PLOTPOLARIZATION_H
#define PLOTPOLARIZATION_H

#include <QWidget>
#include <QTimer>
#include "qcustomplot.h"
#include "typelist.h"
#include "gr/workers/ppworker.h"


/*
Растр вид сбоку
*/

namespace Ui {
class PlotPolarization;
}

class PlotPolarization : public QWidget
{
    Q_OBJECT
    QThread mathThread;

public:
    explicit PlotPolarization(QWidget *parent = 0);
    ~PlotPolarization();
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
    void on_rasterPeriod_valueChanged(int arg1);

    void on_lay_valueChanged(int arg1);

    void on_horizontalSlider_valueChanged(int value);

private:
    Ui::PlotPolarization *ui;
    QCustomPlot *customPlot;
    QCPColorMap *colorMap;
    int Size, Barier, ArgMin, ArgMax, PhMin, PhMax, sharedShip;
    int leRasterPeriod, Lay;
    QString Type;
    Clowd Amp, Phase;
    PPworker *worker;
    QTimer *sharedTimer;
    bool isShared;
};

#endif // PLOTPOLARIZATION_H
