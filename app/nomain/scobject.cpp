#include "scobject.h"
#include "memory.h"

ScObject::ScObject(QObject *parent)
    :QObject(parent),
    m_count(0),
    maxColor(0)
{
    clearData();
    loadDataImage();
    Size = 0;
    AngleMin = 0;
    AngleMax = 0;
    OffsetMin = 0;
    OffsetMax = 0;
    delY = 1.0f;
    plotWork = false;
    rePlot = false;
    isShared = false;
    sharedTimer = new QTimer;
    sharedTimer->setInterval(40);
    connect(sharedTimer,&QTimer::timeout,this,&ScObject::sharedNow);
    worker = new ScWorker();
    worker->moveToThread(&mathThread);
    connect(&mathThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this,&ScObject::syncSignal,worker,&ScWorker::syncSlot);
    connect(this,&ScObject::dataSignal,worker,&ScWorker::dataSlot);
    connect(this,&ScObject::sharedSignal,worker,&ScWorker::sharedSlot);
    connect(this,&ScObject::plotSignal,worker,&ScWorker::plotSlot);
    connect(worker,&ScWorker::plot,this,&ScObject::result);
    mathThread.start();
}
ScObject::~ScObject(){
    mathThread.quit();
    mathThread.wait();
}
void ScObject::sharedNow(){
    if(isShared){
        isShared = false;
    }
    else return;
    emit sharedSignal(sharedShip,tName);
}
void ScObject::shared(int shp){
    isShared = true;
    sharedShip = shp;
    if(!sharedTimer->isActive()){
        sharedTimer->start();
    }
}

void ScObject::setBuf(Clowd &dataA,Clowd &dataH){
    if(sharedTimer->isActive())
        sharedTimer->stop();
    histA = dataA;
    histY = dataH;
    emit dataSignal(histA,histY);
    //emit plotSignal();
}
void ScObject::result(Clowd &data){
    m_data = data;
    m_count = m_data.size();
    emit plot();
}


void ScObject::sync(){
    unsigned int nSize = (int)Memory::get("Size",0).toInt();
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
    delY = (GLfloat)32.0f/(181-Memory::get("PhMin",0).toInt());
    emit syncSignal();
    reLine();
    emit plotSignal();
}
void ScObject::setType(QString typeName){
    tName = typeName;
}
void ScObject::setAngle(int min,int max){
    AngleMin = min+180;
    AngleMax = max+180;
    delY = (GLfloat)32.0f/(181-max);
    reLine();
}
void ScObject::setOffset(unsigned int min, unsigned int max){
    OffsetMin = min;
    OffsetMax = max;
    reLine();
}
void ScObject::setSizeBlock(unsigned int size){
    Size = size;
    reLine();
}
void ScObject::reLine(){
    if(Size == 0) return;
    l_count = 12*2*6;
    l_data.resize(24*6);
    sObjectLink = l_data.data();
    centerX = (GLfloat)Size/2;
    centerY =  AngleMin + (GLfloat) ( AngleMax - AngleMin ) /2;
    centerZ = OffsetMin + (GLfloat) ( OffsetMax - OffsetMin ) /2;
    //top
    appendPoint(-1,AngleMin-1,OffsetMin-1);
    appendPoint(-1,AngleMin-1,OffsetMax+1);
    appendPoint(-1,AngleMin-1,OffsetMin-1);
    appendPoint(-1,AngleMax+1,OffsetMin-1);
    appendPoint(-1,AngleMax+1,OffsetMax+1);
    appendPoint(-1,AngleMax+1,OffsetMin-1);
    appendPoint(-1,AngleMax+1,OffsetMax+1);
    appendPoint(-1,AngleMin-1,OffsetMax+1);

    appendPoint(Size+1,AngleMin-1,OffsetMin-1);
    appendPoint(Size+1,AngleMin-1,OffsetMax+1);
    appendPoint(Size+1,AngleMin-1,OffsetMin-1);
    appendPoint(Size+1,AngleMax+1,OffsetMin-1);
    appendPoint(Size+1,AngleMax+1,OffsetMax+1);
    appendPoint(Size+1,AngleMax+1,OffsetMin-1);
    appendPoint(Size+1,AngleMax+1,OffsetMax+1);
    appendPoint(Size+1,AngleMin-1,OffsetMax+1);

    appendPoint(-1,AngleMax+1,OffsetMax+1);
    appendPoint(Size+1,AngleMax+1,OffsetMax+1);

    appendPoint(-1,AngleMin-1,OffsetMax+1);
    appendPoint(Size+1,AngleMin-1,OffsetMax+1);

    appendPoint(-1,AngleMax+1,OffsetMin-1);
    appendPoint(Size+1,AngleMax+1,OffsetMin-1);

    appendPoint(-1,AngleMin-1,OffsetMin-1);
    appendPoint(Size+1,AngleMin-1,OffsetMin-1);

}
void ScObject::appendPoint(int x,int y,int z){
    *(sObjectLink++) = (centerX-x)/MAXBYTEFLOAT;
    *(sObjectLink++) = (centerY-y)/(MAXBYTEFLOAT*delY);
    *(sObjectLink++) = -(centerZ-z)/MAXBYTEFLOAT;
    *(sObjectLink++) = 0.0f;
    *(sObjectLink++) = 1.0f;
    *(sObjectLink++) = 0.0f;
}
void ScObject::setMax(int color){
    maxColor = color;
    m_data.clear();
    m_count = 0;
    loadDataImage();
}

void ScObject::loadDataImage(){
    /*m_data = c_data;
    int size = m_data.size();
    m_count = size;*/
}
void ScObject::editData(const Clowd &result){
    c_data = result;
    loadDataImage();

}
void ScObject::clearData(){
    c_data.clear();
}
void ScObject::appendPointObj(int x,int y,int z, int color){
    *(m_dataLink++) = (centerX-x)/MAXBYTEFLOAT;
    *(m_dataLink++) = (centerY-y)/(MAXBYTEFLOAT*delY);
    *(m_dataLink++) = -(centerZ-z)/MAXBYTEFLOAT;
    if(color == 0) {
        *(m_dataLink++) = 0;
        *(m_dataLink++) = 0;
        *(m_dataLink++) = 0;
    }
    else {
        *(m_dataLink++) = (float)color/MAXBYTEFLOAT;
        *(m_dataLink++) = 0.0f;
        *(m_dataLink++) = (float)(MAXBYTEFLOAT - color)/MAXBYTEFLOAT;
    }
}
void ScObject::plotFlower(int x,int y,int z, unsigned int color){
    double NormalColor = (double)(QUINT16_SIZE);
    int nColor = (int)MAXBYTE*((double)color)/NormalColor;
    if(nColor > MAXBYTE)
        nColor = MAXBYTE;
    //appendPoint(x,y,z,nColor);
    appendPointObj(x-1,y,z-1,nColor);
    appendPointObj(x+1,y,z-1,nColor);
    appendPointObj(x,y+1,z,nColor);
    appendPointObj(x-1,y,z+1,nColor);
    appendPointObj(x+1,y,z+1,nColor);
    appendPointObj(x,y+1,z,nColor);
    appendPointObj(x+1,y,z-1,nColor);
    appendPointObj(x+1,y,z+1,nColor);
    appendPointObj(x,y+1,z,nColor);
    appendPointObj(x-1,y,z-1,nColor);
    appendPointObj(x-1,y,z+1,nColor);
    appendPointObj(x,y+1,z,nColor);

    appendPointObj(x-1,y,z-1,nColor);
    appendPointObj(x+1,y,z-1,nColor);
    appendPointObj(x,y-1,z,nColor);
    appendPointObj(x-1,y,z+1,nColor);
    appendPointObj(x+1,y,z+1,nColor);
    appendPointObj(x,y-1,z,nColor);
    appendPointObj(x+1,y,z-1,nColor);
    appendPointObj(x+1,y,z+1,nColor);
    appendPointObj(x,y-1,z,nColor);
    appendPointObj(x-1,y,z-1,nColor);
    appendPointObj(x-1,y,z+1,nColor);
    appendPointObj(x,y-1,z,nColor);
}
