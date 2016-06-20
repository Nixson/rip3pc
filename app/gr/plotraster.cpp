#include "plotraster.h"
#include "ui_plotraster.h"
#include "memory.h"

PlotRaster::PlotRaster(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotRaster)
{
    ui->setupUi(this);
    customPlot = ui->plot;
    customPlot->setInteractions(QCP::iRangeZoom); //QCP::iRangeDrag| this will also allow rescaling the color scale by dragging/zooming
    customPlot->axisRect()->setupFullAxesBox(true);
    customPlot->xAxis->setLabel("Отсчеты");
    customPlot->yAxis->setLabel("Отсчеты");
    colorMap = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);
    customPlot->addPlottable(colorMap);
    syncSlot();
    worker = new PRworker();
    worker->moveToThread(&mathThread);
    connect(&mathThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this,&PlotRaster::syncSignal,worker,&PRworker::syncSlot);
    connect(this,&PlotRaster::dataSignal,worker,&PRworker::dataSlot);
    connect(this,&PlotRaster::sharedSignal,worker,&PRworker::sharedSlot);
    connect(this,&PlotRaster::plotSignal,worker,&PRworker::plotSlot);
    connect(worker,&PRworker::plot,this,&PlotRaster::result);
    mathThread.start();
    sharedTimer = new QTimer();
    sharedTimer->setInterval(40);
    connect(sharedTimer,&QTimer::timeout,this,&PlotRaster::sharedNow);
    isShared = false;
}


void PlotRaster::sharedNow(){
    if(isShared){
        isShared = false;
    }
    else return;
    emit sharedSignal(sharedShip,Type);
}
void PlotRaster::shared(int shp){
    isShared = true;
    sharedShip = shp;
    if(!sharedTimer->isActive()){
        sharedTimer->start();
    }
}


PlotRaster::~PlotRaster()
{
    delete ui;
    mathThread.quit();
    mathThread.wait();
}

void PlotRaster::syncSlot(){
    ArgMin = Memory::get("ArgMin",0).toInt();
    ArgMax = Memory::get("ArgMax",1024).toInt();
    PhMin = 0;
    PhMax = Memory::get("PhMin",180).toInt();
    leRasterPeriod = Memory::get("leRasterPeriod"+Type,10).toInt();
    Lay = Memory::get("PlotPLay"+Type,0).toInt();
    Barier = Memory::get("Barier",0).toInt();
    int size = Memory::get("Size",400).toInt();
    if(size!=Size){
        Size = size;
    }
    emit syncSignal(Type);
    emit plotSignal();
//    plot();
}
void PlotRaster::setType(QString type){
    Type = type;
}
void PlotRaster::setBuf(Clowd &Ar, Clowd &Ph){
    if(sharedTimer->isActive()){
        sharedTimer->stop();
    }
    Amp = Ar;
    Phase = Ph;
    emit dataSignal(Ar, Ph);
}
void PlotRaster::result(QCPColorMapData *data){
    colorMap->setData(data);
    colorMap->setGradient(QCPColorGradient::gpThermal);
    colorMap->rescaleDataRange();
    customPlot->rescaleAxes();
    customPlot->replot();
}
void PlotRaster::plot(){
    emit plotSignal();
}


void PlotRaster::on_rbAmp_toggled(bool checked)
{
    Memory::set("rbAmp"+Type,checked);
    emit syncSignal(Type);
    emit plotSignal();
}

void PlotRaster::on_rbPh_toggled(bool checked)
{
    Memory::set("rbPh"+Type,checked);
    emit syncSignal(Type);
    emit plotSignal();
}
