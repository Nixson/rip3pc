#ifndef PLOTOSC_H
#define PLOTOSC_H

#include <QWidget>
#include <QTimer>
#include "qcustomplot.h"

namespace Ui {
class PlotOsc;
}

class PlotOsc : public QWidget
{
    Q_OBJECT

public:
    explicit PlotOsc(QWidget *parent = 0);
    ~PlotOsc();
    void setType(QString type);
    void shared(int);

signals:
    void sync();
    void syncSignal(QString type);

public slots:
    void syncSlot();
    void plot();
    void plotReal();
    void sharedNow();
protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
private:
    Ui::PlotOsc *ui;
    QString Type;
    int ArgMin, ArgMax, PhMin, PhMax, Max;
    QTimer *plotTimer;
    int SharedPosition;
    int Width, Height;
    QColor textColor,linesColor,graphColor,whiteColor,greenColor;
    void plotLines(QPainter &painter);
    void plotInfo(QPainter &painter);
    void plotSubLines(QPainter &painter);
    QCustomPlot *customPlot;
};

#endif // PLOTOSC_H
