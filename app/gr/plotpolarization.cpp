#include "plotpolarization.h"
#include "ui_plotpolarization.h"
#include "memory.h"

PlotPolarization::PlotPolarization(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotPolarization)
{
    ui->setupUi(this);
    customPlot = ui->plot;
    customPlot->setInteractions(QCP::iRangeZoom); //QCP::iRangeDrag| this will also allow rescaling the color scale by dragging/zooming
    customPlot->axisRect()->setupFullAxesBox(true);
    customPlot->xAxis->setLabel("Градусы");
    customPlot->yAxis->setLabel("Отсчеты");
    colorMap = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);
    customPlot->addPlottable(colorMap);
    syncSlot();
    worker = new PPworker();
    worker->moveToThread(&mathThread);
    connect(&mathThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this,&PlotPolarization::syncSignal,worker,&PPworker::syncSlot);
    connect(this,&PlotPolarization::dataSignal,worker,&PPworker::dataSlot);
    connect(this,&PlotPolarization::sharedSignal,worker,&PPworker::sharedSlot);
    connect(this,&PlotPolarization::plotSignal,worker,&PPworker::plotSlot);
    connect(worker,&PPworker::plot,this,&PlotPolarization::result);
    mathThread.start();
    sharedTimer = new QTimer();
    sharedTimer->setInterval(40);
    connect(sharedTimer,&QTimer::timeout,this,&PlotPolarization::sharedNow);
    isShared = false;
}

void PlotPolarization::sharedNow(){
    if(isShared){
        isShared = false;
        sharedTimer->stop();
    }
    else return;
    emit sharedSignal(sharedShip,Type);
}
void PlotPolarization::shared(int shp){
    isShared = true;
    sharedShip = shp;
    if(!sharedTimer->isActive()){
        sharedTimer->start();
    }
}

PlotPolarization::~PlotPolarization()
{
    delete ui;
    mathThread.quit();
    mathThread.wait();
}
void PlotPolarization::syncSlot(){
    ArgMin = Memory::get("ArgMin",0).toInt();
    ArgMax = Memory::get("ArgMax",1024).toInt();
    PhMin = 0;
    PhMax = Memory::get("PhMin",180).toInt();
    leRasterPeriod = Memory::get("leRasterPeriod"+Type,10).toInt();
    Lay = Memory::get("PlotPLay"+Type,0).toInt();
    Barier = Memory::get("Barier",0).toInt();
    int size = Memory::get("Size",400).toInt();
    if(size!=Size){
        ui->horizontalSlider->setMaximum(size);
        ui->lay->setMaximum(size);
        Size = size;
    }
    emit syncSignal(Type);
    emit plotSignal();
//    plot();
}
void PlotPolarization::setType(QString type){
    Type = type;
}
void PlotPolarization::setBuf(Clowd &Ar, Clowd &Ph){
    if(sharedTimer->isActive()){
        sharedTimer->stop();
    }
    Amp = Ar;
    Phase = Ph;
    emit dataSignal(Ar, Ph);
}
void PlotPolarization::result(QCPColorMapData *data){
    colorMap->setData(data);
    colorMap->setGradient(QCPColorGradient::gpThermal);
    colorMap->rescaleDataRange();
    customPlot->rescaleAxes();
    customPlot->replot();
}
void PlotPolarization::plot(){
    emit plotSignal();
}

void PlotPolarization::on_rasterPeriod_valueChanged(int arg1)
{
    leRasterPeriod = arg1;
    Memory::set("leRasterPeriod"+Type,arg1);
    emit sync();
    emit syncSignal(Type);
}

void PlotPolarization::on_lay_valueChanged(int arg1)
{
    Lay = arg1;
    Memory::set("PlotPLay"+Type,arg1);
    emit sync();
    emit syncSignal(Type);
}

void PlotPolarization::on_horizontalSlider_valueChanged(int value)
{
    Lay = value;
    Memory::set("PlotPLay"+Type,value);
    emit sync();
    emit syncSignal(Type);
}
