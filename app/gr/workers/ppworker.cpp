#include "ppworker.h"
#include "memory.h"

PPworker::PPworker(QObject *parent) : QObject(parent)
{
}
void PPworker::syncSlot(QString type){
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
}
void PPworker::sharedSlot(int ship, QString type){
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
void PPworker::dataSlot(Clowd &dataA, Clowd &dataH){
    histA = dataA;
    histY = dataH;
    plotSlot();
}
void PPworker::plotSlot(){
    if(histA.size()==0){
        return;
    }
    int nx =AngleMax - AngleMin;
    int ny =OffsetMax - OffsetMin;
    QCPColorMapData *data = new QCPColorMapData(nx,ny,QCPRange(AngleMin, AngleMax), QCPRange(OffsetMin, OffsetMax));
    double x, y;
    float Max = 0.0;
    if(histY.size() >= Size*BLOCKLANGTH){
        for (int xIndex=0; xIndex<nx; ++xIndex)
        {
          for (int yIndex=0; yIndex<ny; ++yIndex)
          {
            data->cellToCoord(xIndex, yIndex, &x, &y);
            int rY = (int)(x+180);
            int rZ = (int)round(y);

            float A = 0;
            int P = 0;
            int Step = 0;
            for(P = Lay, Step = 0; Step <= leRasterPeriod && P < Size; ++Step, ++P){
                if((int)histY[P*BLOCKLANGTH+rZ] == rY){
                    double AmpR = (double)histA[P*BLOCKLANGTH+rZ];
                    if(AmpR >= MaxBarier)
                        A+= (AmpR - MaxBarier);
                }
            }
            if(Max < A) Max = A;
            data->setCell(xIndex, yIndex, A);
          }
        }
    }
    emit plot(data);
}
