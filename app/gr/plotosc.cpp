#include "plotosc.h"
#include "ui_plotosc.h"
#include "memory.h"
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <iostream>
#include <QPaintEvent>

PlotOsc::PlotOsc(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotOsc)
{
    ui->setupUi(this);
    plotTimer = new QTimer();
    plotTimer->setInterval(40);
    connect(plotTimer,&QTimer::timeout,this,&PlotOsc::plotReal);
    /*Width = ui->painer->width();                                              Свой график
    Height = ui->painer->height();
    textColor = QColor(0,0,0,200);
    linesColor = QColor(0,0,100,200);
    graphColor = QColor(250,0,0,250);
    whiteColor = QColor(255,255,255,255);
    greenColor = QColor(0,100,0,200);*/

    customPlot = ui->painer;
    customPlot->setInteractions(QCP::iRangeZoom); //QCP::iRangeDrag| this will also allow rescaling the color scale by dragging/zooming
    customPlot->axisRect()->setupFullAxesBox(true);
    customPlot->xAxis->setLabel("Отсчеты");
    customPlot->yAxis->setLabel("Отсчеты");
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(Qt::red));
    customPlot->graph(0)->setBrush(QBrush(QColor(200, 0, 0, 150)));
    customPlot->graph(0)->rescaleAxes();


    plot();
    Max = 1000;
    //ui->pLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);        Свой график
}

PlotOsc::~PlotOsc()
{
    delete ui;
}
void PlotOsc::setType(QString type){
    Type = type;
}
void PlotOsc::syncSlot(){
    ArgMin = Memory::get("ArgMin",0).toInt();
    ArgMax = Memory::get("ArgMax",1024).toInt();
    PhMin = 0;
    PhMax = Memory::get("PhMin",180).toInt();
    customPlot->xAxis->setRange(ArgMin, ArgMax);
    customPlot->yAxis->setRange(0, 1);
}
void PlotOsc::shared(int num){
    SharedPosition = num;
    plot();
}
void PlotOsc::sharedNow(){}
void PlotOsc::plot(){
    if(plotTimer->isActive()){
        return;
    }
    else {
        plotTimer->start();
    }
}
void PlotOsc::resizeEvent(QResizeEvent *event){
    Q_UNUSED(event);
    /*Width = ui->painer->width();                                              Свой график
    Height = ui->painer->height();
    plot();*/
}
void PlotOsc::paintEvent(QPaintEvent *event){
    Q_UNUSED(event);
}
void PlotOsc::plotReal(){
    plotTimer->stop();
    if(SharedPosition < 1) return;
    unsigned int start = ((unsigned int)SharedPosition-1)*BLOCKLANGTH*sizeof(float);
    int range = BLOCKLANGTH*sizeof(float);
    floatVector data;
    data.resize(BLOCKLANGTH);
    Memory::getData( Type=="gorizontal" ? "vGorizontalAr" : "vVerticalAr",data.data(),range,start);

    //
    float max = 0.0;
    int pos = 0;
    int sPos = 0;
    MathVector x,y;
    x.resize((ArgMax-ArgMin+1));
    y.resize((ArgMax-ArgMin+1));

    for(auto dIter = data.begin();dIter!=data.end();++dIter){
        if(pos >= ArgMin && pos <= ArgMax){
            if(max < *dIter)
                max = *dIter;
            x[sPos] = pos;
            y[sPos] = (double)*dIter;
            ++sPos;
        }
        ++pos;
    }
    customPlot->yAxis->setRange(0, max*1.09);

    customPlot->graph(0)->setData(x, y);
    customPlot->replot();
    /*if(Width < 50 || Height < 40)
        return;

    QPixmap pixmap( Width, Height );                                              Свой график
    pixmap.fill( Qt::white );
    QPainter painter( &pixmap );
    painter.setFont(QFont("Arial", 10));
    plotInfo(painter);
    plotLines(painter);
    plotSubLines(painter);
    ui->pLabel->setPixmap(pixmap.scaled(Width,Height,Qt::KeepAspectRatio));*/

}
void PlotOsc::plotInfo(QPainter &painter){
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(graphColor);
    unsigned int start = ((unsigned int)SharedPosition-1)*BLOCKLANGTH*sizeof(float);
    int range = BLOCKLANGTH*sizeof(float);
    floatVector data;
    data.resize(BLOCKLANGTH);
    Memory::getData( Type=="gorizontal" ? "vGorizontalAr" : "vVerticalAr",data.data(),range,start);
    float max = 0;
    int pos = 0;
    for(auto dIter = data.begin();dIter!=data.end();++dIter){
        ++pos;
        if(pos >= ArgMin && pos <= ArgMax){
            if(max < *dIter)
                max = *dIter;
        }
    }
    max *=1.09;
    QPainterPath path;
    path.moveTo(40,Height - 20);
    float normalX = ((float)ArgMax - ArgMin)/(Width - 60);
    float normalY = (max)/(Height - 40);
    pos = 0;
    for(auto dIter = data.begin();dIter!=data.end();++dIter){
        ++pos;
        if(pos >= ArgMin && pos <= ArgMax){
            float position = 40 + (pos-ArgMin)/normalX;
            float value = 20 + (max - *dIter)/normalY;
            path.lineTo(position,Height - 20);
            path.lineTo(position,value);
        }

    }
    painter.drawPath( path );
    Max = (int)max;
}
void PlotOsc::plotSubLines(QPainter &painter){
    painter.setRenderHint(QPainter::Antialiasing, false);
    QPen pen = painter.pen();
    pen.setStyle(Qt::DashLine);
    pen.setColor(greenColor);
    pen.setWidth(1);
    painter.setPen(pen);

    QPainterPath path;
    int LinesHeight = Height-40;
    int countNumbsV = LinesHeight/50;
    for(int i = 1; i <= countNumbsV;++i){
        path.moveTo(41,(Height-30) - i*50);
        path.lineTo( Width-21, (Height-30) - i*50);
    }
    int LinesWidth = Width-60;
    int countNumbsG = LinesWidth/50;
    for(int i = 1; i <= countNumbsG;++i){
        path.moveTo(i*50+35,20);
        path.lineTo(i*50+35,Height-20);
    }
    painter.drawPath( path );

}
void PlotOsc::plotLines(QPainter &painter){
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setPen( textColor );
    int LinesHeight = Height-40;
    int countNumbsV = LinesHeight/50;
    if(countNumbsV > 0){
        int start = 0;
        int step = (Max)/countNumbsV;
        for(int i = 0; i <= countNumbsV;++i){
            QRect rect;
            rect.setRect(0,(Height-30) - i*50,35, 50 );
            painter.drawText(rect,Qt::AlignRight,QString::number(start+step*i));
        }
    }
    int LinesWidth = Width-60;
    int countNumbsG = LinesWidth/50;
    if(countNumbsG > 0){
        int start = ArgMin;
        int step = (ArgMax - ArgMin)/countNumbsG;
        for(int i = 0; i <= countNumbsG;++i){
            QRect rect;
            rect.setRect(i*50+35,Height-15,i*50+70, Height-15 );
            painter.drawText(rect,Qt::AlignLeft,QString::number(start+step*i));
        }
    }


    //painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen( linesColor );


    QPainterPath path;
    path.moveTo(40,20);
    path.lineTo( 40, Height - 20 );
    path.lineTo( Width - 20, Height - 20 );
    path.lineTo( Width - 20, 20 );
    path.lineTo( 40, 20 );
    painter.drawPath( path );
}
