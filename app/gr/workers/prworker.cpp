#include "prworker.h"
#include "memory.h"

PRworker::PRworker(QObject *parent) : QObject(parent)
{
}
void PRworker::syncSlot(QString type){
    int nSize = Memory::get("Size",0).toInt();
    if(nSize != Size){
        Size = nSize;
        histA.clear();
        histA.resize(Size*BLOCKLANGTH);
        histY.clear();
        histY.resize(Size*BLOCKLANGTH);
    }
    AngleMin = 180-Memory::get("PhMin",0).toInt();
    AngleMax = Memory::get("PhMin",0).toInt()+180;
    OffsetMin = Memory::get("ArgMin",0).toInt();
    OffsetMax = Memory::get("ArgMax",1024).toInt();
    maxColor = Memory::get("colorValue",0).toInt();
    MaxBarier = Memory::get("Barier",100).toInt();
    leRasterPeriod = Memory::get("leRasterPeriod"+type,10).toInt();
    Lay = Memory::get("PlotPLay"+type,0).toInt();
    rbAmp = Memory::get("rbAmp"+type,false).toBool();
    rbPh = Memory::get("rbPh"+type,false).toBool();
}
void PRworker::sharedSlot(int ship, QString type){
    if(histA.size()==0){
        syncSlot(type);
    }
    QString nameA,nameP;
    if(type=="Vertical"){
        nameA = "vVerticalAr";
        nameP = "vVerticalPh";
    }
    else{
        nameA = "vGorizontalAr";
        nameP = "vGorizontalPh";
    }
    int shipPosition = ship*BLOCKLANGTH*sizeof(float);
    Memory::getData(nameA,histA.data(),shipPosition);
    Memory::getData(nameP,histY.data(),shipPosition);
    plotSlot();
}
void PRworker::dataSlot(Clowd &dataA, Clowd &dataH){
    histA = dataA;
    histY = dataH;
    plotSlot();
}
void PRworker::plotSlot(){
    if(histA.size()==0){
        return;
    }
    int sz = histA.size();
    if(rbAmp)
        sz = histA.size();
    else if(rbPh)
        sz = histY.size();
    int nx =Size;
    int ny =OffsetMax - OffsetMin;
    QCPColorMapData *data = new QCPColorMapData(nx,ny,QCPRange(0, Size), QCPRange(OffsetMin, OffsetMax));
    double x, y;
    float Max = 0.0;
    if(histY.size() >= Size*BLOCKLANGTH){
        for (int xIndex=0; xIndex<nx; ++xIndex)
        {
          for (int yIndex=0; yIndex<ny; ++yIndex)
          {
            data->cellToCoord(xIndex, yIndex, &x, &y);
            int r = (int)x * BLOCKLANGTH+(int)y;
            float A = 0.0;
            if(rbAmp){
                if(r <= sz)
                    A = (double)histA[r];
            }
            else if(rbPh){
                if(r <= sz)
                    A = (double)histY[r];
            }
            if(Max < A) Max = A;
            data->setCell(xIndex, yIndex, A);
          }
        }
    }
    emit plot(data);
}
