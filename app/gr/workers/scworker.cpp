#include "scworker.h"
#include "memory.h"
#include "math.h"

ScWorker::ScWorker(QObject *parent) : QObject(parent)
{
    syncSlot();
    GLfloat R=0.75; // радиус сферы

       // начальные значения для икосаэдра
    GLfloat a=4*R/sqrt(10+2*sqrt(5)); // сторона икосаэдра
    GLfloat alpha=acos((1-a*a/2/R/R)); // первый угол поворота по тэта
    float pi = 3.14159265358979323846;
    float k = pi/180;

       // cos(alpha)=(1-a*a/2/R/R)

       // вычисляем точки икосаэдра
       //0 точка
       VertexArray[0][0]=0;   // x
       VertexArray[0][1]=0;   // y
       VertexArray[0][2]=R;   // z

       //1 точка
       VertexArray[1][0]=R*sin(alpha)*sin(0);
       VertexArray[1][1]=R*sin(alpha)*cos(0);
       VertexArray[1][2]=R*cos(alpha);

       //2 точка
       VertexArray[2][0]=R*sin(alpha)*sin(72*k);
       VertexArray[2][1]=R*sin(alpha)*cos(72*k);
       VertexArray[2][2]=R*cos(alpha);

       //3 точка
       VertexArray[3][0]=R*sin(alpha)*sin(2*72*k);
       VertexArray[3][1]=R*sin(alpha)*cos(2*72*k);
       VertexArray[3][2]=R*cos(alpha);

       //4 точка
       VertexArray[4][0]=R*sin(alpha)*sin(3*72*k);
       VertexArray[4][1]=R*sin(alpha)*cos(3*72*k);
       VertexArray[4][2]=R*cos(alpha);

       //5 точка
       VertexArray[5][0]=R*sin(alpha)*sin(4*72*k);
       VertexArray[5][1]=R*sin(alpha)*cos(4*72*k);
       VertexArray[5][2]=R*cos(alpha);

       //6 точка
       VertexArray[6][0]=R*sin(pi-alpha)*sin(-36*k);
       VertexArray[6][1]=R*sin(pi-alpha)*cos(-36*k);
       VertexArray[6][2]=R*cos(pi-alpha);

       //7 точка
       VertexArray[7][0]=R*sin(pi-alpha)*sin(36*k);
       VertexArray[7][1]=R*sin(pi-alpha)*cos(36*k);
       VertexArray[7][2]=R*cos(pi-alpha);

       //8 точка
       VertexArray[8][0]=R*sin(pi-alpha)*sin((36+72)*k);
       VertexArray[8][1]=R*sin(pi-alpha)*cos((36+72)*k);
       VertexArray[8][2]=R*cos(pi-alpha);

       //9 точка
       VertexArray[9][0]=R*sin(pi-alpha)*sin((36+2*72)*k);
       VertexArray[9][1]=R*sin(pi-alpha)*cos((36+2*72)*k);
       VertexArray[9][2]=R*cos(pi-alpha);

       //10 точка
       VertexArray[10][0]=R*sin(pi-alpha)*sin((36+3*72)*k);
       VertexArray[10][1]=R*sin(pi-alpha)*cos((36+3*72)*k);
       VertexArray[10][2]=R*cos(pi-alpha);

       //11 точка
       VertexArray[11][0]=0;
       VertexArray[11][1]=0;
       VertexArray[11][2]=-R;
}
void ScWorker::syncSlot(){
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
    delY = (GLfloat)32.0f/(181-Memory::get("PhMin",0).toInt());

    centerX = (GLfloat)Size/2;
    centerY =  AngleMin + (GLfloat) ( AngleMax - AngleMin ) /2;
    centerZ = OffsetMin + (GLfloat) ( OffsetMax - OffsetMin ) /2;
}
void ScWorker::sharedSlot(int ship, QString type){
    if(histA.size()==0){
        syncSlot();
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
void ScWorker::dataSlot(Clowd &dataA, Clowd &dataH){
    histA = dataA;
    histY = dataH;
    plotSlot();
}
void ScWorker::plotSlot(){
    if(histA.size() == 0)
        return;
    centerX = (GLfloat)Size/2;
    centerY =  AngleMin + (GLfloat) ( AngleMax - AngleMin ) /2;
    centerZ = OffsetMin + (GLfloat) ( OffsetMax - OffsetMin ) /2;
    int realSize = 0;
    m_data.clear();
    //размер = длина * ширина * 24 точки по 6 координат в каждой
    //m_data.resize( (OffsetMax-OffsetMin) * Size * 144);
    m_data.resize( (OffsetMax-OffsetMin) * Size * 60*6);
    m_dataLink = m_data.data();

    for( int x = 0; x < Size; x++){
        //номер пачки
        int packet = x*BLOCKLANGTH;
        for(int z = OffsetMin; z < OffsetMax; z++){
            int y = (int)histY[packet+z];
            if(y >= AngleMin && y <= AngleMax){
                uint color = (uint)histA[packet+z];
                if(color >= MaxBarier){
                    plotFlower((int)x,(int)y,(int)z,color);
                    realSize++;
                }
            }
        }

    }
//    m_data.resize(realSize*24*6);
        m_data.resize(realSize*60*6);
    emit plot(m_data);
}
void ScWorker::plotFlower(int x,int y,int z, uint color){
    double NormalColor = (double)(QUINT16_SIZE);
    int nColor = (int)MAXBYTE*((double)color)/NormalColor;
    if(nColor > MAXBYTE)
        nColor = MAXBYTE;
    //appendPoint(x,y,z,nColor);

    /*appendPointObj(x-1,y,z-1,nColor);
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
    appendPointObj(x,y-1,z,nColor);*/


    appendPointObj((float)x-VertexArray[0][0],(float)y-VertexArray[0][1],(float)z-VertexArray[0][2],nColor);
    appendPointObj((float)x-VertexArray[2][0],(float)y-VertexArray[2][1],(float)z-VertexArray[2][2],nColor);
    appendPointObj((float)x-VertexArray[1][0],(float)y-VertexArray[1][1],(float)z-VertexArray[1][2],nColor);

    appendPointObj((float)x-VertexArray[0][0],(float)y-VertexArray[0][1],(float)z-VertexArray[0][2],nColor);
    appendPointObj((float)x-VertexArray[3][0],(float)y-VertexArray[3][1],(float)z-VertexArray[3][2],nColor);
    appendPointObj((float)x-VertexArray[2][0],(float)y-VertexArray[2][1],(float)z-VertexArray[2][2],nColor);

       // 2 треугольник
    appendPointObj((float)x-VertexArray[0][0],(float)y-VertexArray[0][1],(float)z-VertexArray[0][2],nColor);
    appendPointObj((float)x-VertexArray[4][0],(float)y-VertexArray[4][1],(float)z-VertexArray[4][2],nColor);
    appendPointObj((float)x-VertexArray[3][0],(float)y-VertexArray[3][1],(float)z-VertexArray[3][2],nColor);

       // 3 треугольник
    appendPointObj((float)x-VertexArray[0][0],(float)y-VertexArray[0][1],(float)z-VertexArray[0][2],nColor);
    appendPointObj((float)x-VertexArray[5][0],(float)y-VertexArray[5][1],(float)z-VertexArray[5][2],nColor);
    appendPointObj((float)x-VertexArray[4][0],(float)y-VertexArray[4][1],(float)z-VertexArray[4][2],nColor);

       // 4 треугольник
    appendPointObj((float)x-VertexArray[0][0],(float)y-VertexArray[0][1],(float)z-VertexArray[0][2],nColor);
    appendPointObj((float)x-VertexArray[1][0],(float)y-VertexArray[1][1],(float)z-VertexArray[1][2],nColor);
    appendPointObj((float)x-VertexArray[5][0],(float)y-VertexArray[5][1],(float)z-VertexArray[5][2],nColor);

       // 5 треугольник
    appendPointObj((float)x-VertexArray[6][0],(float)y-VertexArray[6][1],(float)z-VertexArray[6][2],nColor);
    appendPointObj((float)x-VertexArray[1][0],(float)y-VertexArray[1][1],(float)z-VertexArray[1][2],nColor);
    appendPointObj((float)x-VertexArray[7][0],(float)y-VertexArray[7][1],(float)z-VertexArray[7][2],nColor);

       // 6 треугольник
    appendPointObj((float)x-VertexArray[7][0],(float)y-VertexArray[7][1],(float)z-VertexArray[7][2],nColor);
    appendPointObj((float)x-VertexArray[1][0],(float)y-VertexArray[1][1],(float)z-VertexArray[1][2],nColor);
    appendPointObj((float)x-VertexArray[2][0],(float)y-VertexArray[2][1],(float)z-VertexArray[2][2],nColor);

       // 7 треугольник
    appendPointObj((float)x-VertexArray[7][0],(float)y-VertexArray[7][1],(float)z-VertexArray[7][2],nColor);
    appendPointObj((float)x-VertexArray[2][0],(float)y-VertexArray[2][1],(float)z-VertexArray[2][2],nColor);
    appendPointObj((float)x-VertexArray[8][0],(float)y-VertexArray[8][1],(float)z-VertexArray[8][2],nColor);

       // 8 треугольник
    appendPointObj((float)x-VertexArray[8][0],(float)y-VertexArray[8][1],(float)z-VertexArray[8][2],nColor);
    appendPointObj((float)x-VertexArray[2][0],(float)y-VertexArray[2][1],(float)z-VertexArray[2][2],nColor);
    appendPointObj((float)x-VertexArray[3][0],(float)y-VertexArray[3][1],(float)z-VertexArray[3][2],nColor);

       // 9 треугольник
    appendPointObj((float)x-VertexArray[8][0],(float)y-VertexArray[8][1],(float)z-VertexArray[8][2],nColor);
    appendPointObj((float)x-VertexArray[3][0],(float)y-VertexArray[3][1],(float)z-VertexArray[3][2],nColor);
    appendPointObj((float)x-VertexArray[9][0],(float)y-VertexArray[9][1],(float)z-VertexArray[9][2],nColor);

       // 10 треугольник
    appendPointObj((float)x-VertexArray[9][0],(float)y-VertexArray[9][1],(float)z-VertexArray[9][2],nColor);
    appendPointObj((float)x-VertexArray[3][0],(float)y-VertexArray[3][1],(float)z-VertexArray[3][2],nColor);
    appendPointObj((float)x-VertexArray[4][0],(float)y-VertexArray[4][1],(float)z-VertexArray[4][2],nColor);

       // 11 треугольник
    appendPointObj((float)x-VertexArray[9][0],(float)y-VertexArray[9][1],(float)z-VertexArray[9][2],nColor);
    appendPointObj((float)x-VertexArray[4][0],(float)y-VertexArray[4][1],(float)z-VertexArray[4][2],nColor);
    appendPointObj((float)x-VertexArray[10][0],(float)y-VertexArray[10][1],(float)z-VertexArray[10][2],nColor);

       // 12 треугольник
    appendPointObj((float)x-VertexArray[10][0],(float)y-VertexArray[10][1],(float)z-VertexArray[10][2],nColor);
    appendPointObj((float)x-VertexArray[4][0],(float)y-VertexArray[4][1],(float)z-VertexArray[4][2],nColor);
    appendPointObj((float)x-VertexArray[5][0],(float)y-VertexArray[5][1],(float)z-VertexArray[5][2],nColor);

       // 13 треугольник
    appendPointObj((float)x-VertexArray[10][0],(float)y-VertexArray[10][1],(float)z-VertexArray[10][2],nColor);
    appendPointObj((float)x-VertexArray[5][0],(float)y-VertexArray[5][1],(float)z-VertexArray[5][2],nColor);
    appendPointObj((float)x-VertexArray[6][0],(float)y-VertexArray[6][1],(float)z-VertexArray[6][2],nColor);

       // 14 треугольник
    appendPointObj((float)x-VertexArray[6][0],(float)y-VertexArray[6][1],(float)z-VertexArray[6][2],nColor);
    appendPointObj((float)x-VertexArray[5][0],(float)y-VertexArray[5][1],(float)z-VertexArray[5][2],nColor);
    appendPointObj((float)x-VertexArray[1][0],(float)y-VertexArray[1][1],(float)z-VertexArray[1][2],nColor);

       // 15 треугольник
    appendPointObj((float)x-VertexArray[7][0],(float)y-VertexArray[7][1],(float)z-VertexArray[7][2],nColor);
    appendPointObj((float)x-VertexArray[11][0],(float)y-VertexArray[11][1],(float)z-VertexArray[11][2],nColor);
    appendPointObj((float)x-VertexArray[6][0],(float)y-VertexArray[6][1],(float)z-VertexArray[6][2],nColor);

       // 16 треугольник
    appendPointObj((float)x-VertexArray[8][0],(float)y-VertexArray[8][1],(float)z-VertexArray[8][2],nColor);
    appendPointObj((float)x-VertexArray[11][0],(float)y-VertexArray[11][1],(float)z-VertexArray[11][2],nColor);
    appendPointObj((float)x-VertexArray[7][0],(float)y-VertexArray[7][1],(float)z-VertexArray[7][2],nColor);

       // 17 треугольник
    appendPointObj((float)x-VertexArray[9][0],(float)y-VertexArray[9][1],(float)z-VertexArray[9][2],nColor);
    appendPointObj((float)x-VertexArray[11][0],(float)y-VertexArray[11][1],(float)z-VertexArray[11][2],nColor);
    appendPointObj((float)x-VertexArray[8][0],(float)y-VertexArray[8][1],(float)z-VertexArray[8][2],nColor);

       // 18 треугольник
    appendPointObj((float)x-VertexArray[10][0],(float)y-VertexArray[10][1],(float)z-VertexArray[10][2],nColor);
    appendPointObj((float)x-VertexArray[11][0],(float)y-VertexArray[11][1],(float)z-VertexArray[11][2],nColor);
    appendPointObj((float)x-VertexArray[9][0],(float)y-VertexArray[9][1],(float)z-VertexArray[9][2],nColor);

       // 19 треугольник
    appendPointObj((float)x-VertexArray[6][0],(float)y-VertexArray[6][1],(float)z-VertexArray[6][2],nColor);
    appendPointObj((float)x-VertexArray[11][0],(float)y-VertexArray[11][1],(float)z-VertexArray[11][2],nColor);
    appendPointObj((float)x-VertexArray[10][0],(float)y-VertexArray[10][1],(float)z-VertexArray[10][2],nColor);
}
void ScWorker::appendPointObj(float x,float y,float z, int color){
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
