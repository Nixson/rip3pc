#include "worker.h"
#include "memory.h"
#include <QDataStream>
#include <QDateTime>
#include <QFile>
#include <iostream>
using namespace std;


#if !defined(M_PI)
#define M_PI		3.14159265358979323846
#endif

Worker::Worker(QObject *parent) : QObject(parent)
{
    p_udpSocket = new QUdpSocket();
    PacketNum = 0;
    udp = new UDPSock();
    udp->setAutoDelete(false);
//    connect(this,&Worker::SendCmdPacket,udp,&UDPSock::SendCmdPacket);
    connect(this,&Worker::updateInterface,udp,&UDPSock::updateInterface);
    OriginalPulseRe = new float[BLOCKLANGTH];
    OriginalPulseIm = new float[BLOCKLANGTH];
    isAttach = false;
    maxColor = 0.0;
    colorStep = 0.0;
    tp = QThreadPool::globalInstance();
    tp->reserveThread();
    tp->start(udp);

//    connect(&udpThread, &QThread::finished, udp, &QObject::deleteLater);
//    udpThread.start();
}
Worker::~Worker(){
    tp->releaseThread();
    p_udpSocket->deleteLater();
    /*tcpThread.quit();
    tcpThread.wait();*/
}
void Worker::loadSrc(QByteArray &data){
    Memory::clearData();
    int step = BLOCKLANGTH*32; // определяем количество отсчетов
    int cntD = data.count();
    initPulse(Memory::get("leSubBufNum",0).toInt(),Memory::get("leFreq",0.0).toDouble());
    int bSize = cntD/step;
    int bNum[4] = {0, 16, 15, 31};
    //dataDouble = new double[cntD/8];
    dataFloat.resize(cntD/8);
    float *dataFloatAddr = dataFloat.data();

    for( int bStep = 0; bStep < bSize; bStep++){
        //номер пачки
        int packet = bStep*step;
        // берем 0, 16, 15, 31 блоки
        //0
        for( int pStep = 0; pStep < 4; pStep++){
            for( int sStep = 0; sStep < BLOCKLANGTH; sStep++){
                int vl = (unsigned int)data[packet+bNum[pStep]*BLOCKLANGTH+sStep];
                if(vl < 0) vl += MAXBYTE;
                *dataFloatAddr++ = (GLfloat)(vl - 128);
            }
        }
    }
    Size = bSize;
    Memory::set("Size",bSize);
    emit save();
    Buffer();
    attach();
    run();
    emit log("Обработка завершена");
}
void Worker::loadFinishedF(QByteArray &data){
    int binarySize = data.size()/8;
    Size = binarySize/BLOCKLANGTH;
    Memory::set("Size",Size);
    emit save();
    if(histGA.size()!=binarySize){
        histGA.clear();
        histVA.clear();
        histGY.clear();
        histVY.clear();
        histGA.resize(binarySize);
        histVA.resize(binarySize);
        histGY.resize(binarySize);
        histVY.resize(binarySize);
    }
    char *dataFile = data.data();
    memcpy(histGA.data(),dataFile,data.size()/2);
    memcpy(histVA.data(),dataFile,data.size()/2);
    dataFile += data.size()/2;
    memcpy(histGY.data(),dataFile,data.size()/2);
    memcpy(histVY.data(),dataFile,data.size()/2);
    data.clear();
    emit resultGorizontal(histGA,histGY);
    emit resultVertical(histVA,histVY);
}
void Worker::loadFinished(QByteArray &data){
    int binarySize = data.size()/8;
    Size = binarySize/ArgMax;
    Memory::set("Size",Size);
    emit save();
    if(histGA.size()!=Size*BLOCKLANGTH){
        histGA.clear();
        histVA.clear();
        histGY.clear();
        histVY.clear();
        histGA.resize(Size*BLOCKLANGTH);
        histVA.resize(Size*BLOCKLANGTH);
        histGY.resize(Size*BLOCKLANGTH);
        histVY.resize(Size*BLOCKLANGTH);
    }
    Clowd bsA;
    Clowd bsP;
    bsA.resize(binarySize);
    bsP.resize(binarySize);
    char *dataFile = data.data();
    memcpy(bsA.data(),dataFile,data.size()/2);
    dataFile += binarySize*4;
    memcpy(bsP.data(),dataFile,data.size()/2);
    maxColor = 0.0;
    float paxPh = 0.0;

    for( int step = 0; step < Size; ++step){
        for(int mk = 0; mk < BLOCKLANGTH; ++mk){
            int position = mk+step*BLOCKLANGTH;
            if(mk >= ArgMax){
                histGA[position] = 0;
                histVA[position] = 0;
                histGY[position] = 0;
                histVY[position] = 0;
            }
            else {
                int positionR = mk+step*ArgMax;
                histGA[position] = bsA[positionR];
                histVA[position] = bsA[positionR];
                if(maxColor < bsA[positionR])
                    maxColor = bsA[positionR];
                histGY[position] = 180+bsP[positionR]*180/M_PI;
                histVY[position] = 180+bsP[positionR]*180/M_PI;
                if(paxPh < bsP[positionR])
                    paxPh = bsP[positionR];
            }
        }
    }
    bsA.clear();
    bsP.clear();
    float norm = (float)QUINT16_SIZE/maxColor;

    for( int step = 0; step < Size; ++step){
        for(int mk = 0; mk < BLOCKLANGTH; ++mk){
            int position = mk+step*BLOCKLANGTH;
            float ps = histGA[position]*norm;
            histGA[position] = ps;
            histVA[position] = ps;
        }
    }
    maxColor = QUINT16_SIZE;
    emit resultGorizontal(histGA,histGY);
    emit resultVertical(histVA,histVY);
}
void Worker::sync(){
    ArgMin = Memory::get("ArgMin",0).toInt();
    ArgMax = Memory::get("ArgMax",1024).toInt();
    emit updateInterface();
}
void Worker::attach(){
    if(histGA.size()!=Size*BLOCKLANGTH){
        histGA.clear();
        histGA.resize(Size*BLOCKLANGTH);
        histVA.clear();
        histVA.resize(Size*BLOCKLANGTH);
        histGY.clear();
        histGY.resize(Size*BLOCKLANGTH);
        histVY.clear();
        histVY.resize(Size*BLOCKLANGTH);
    }
}
void Worker::compile(int startPos,int position,int iNum){
    //float *tmpGA,*tmpVA;
    int numPos = iNum*BLOCKLANGTH;
    //int sInum = iNum;
    for(int bStep = startPos+1; bStep < position; bStep+=2 ){
        histGY[numPos] = (float)round(ResUlst[bStep]);
        histVY[numPos] = (float)round(ResUlstY[bStep]);
        ++numPos;
    }
    float maxColorArg = maxColor*0.7;
    float norm = (float)QUINT16_SIZE/maxColorArg;
/*    float *dataResultIntG = histGA.data();
    float *dataResultIntV = histVA.data();*/
    int start = startPos;
    numPos = iNum*BLOCKLANGTH;
    if(maxColor != colorStep){
        numPos = 0;
        start = 0;
        //sInum = 0;
        colorStep = maxColor;
    }
    for(int bStep = start; bStep < position; bStep+=2){
        float rsp = (float)round(ResUlst[bStep]*norm);
        if(rsp >= (float)QUINT16_SIZE)
            rsp = (float)QUINT16_SIZE-1.0;
        histGA[numPos] = rsp;
        rsp = (float)round(ResUlstY[bStep]*norm);
        if(rsp >= (float)QUINT16_SIZE)
            rsp = (float)QUINT16_SIZE-1.0;
        histVA[numPos] = rsp;
        ++numPos;
    }

    int progressValue = 100*iNum/Size;
    if(progressValue > lastProgress){
        lastProgress = progressValue;
        emit progress(lastProgress);
            int range = numPos*sizeof(float);
            Memory::setData("vGorizontalAr",histGA.data(),range);
            Memory::setData("vGorizontalPh",histGY.data(),range);

            Memory::setData("vVerticalAr",histVA.data(),range);
            Memory::setData("vVerticalPh",histVY.data(),range);
            emit shared(iNum);
    }
}
void Worker::run(){
    int sizeResp = Size*BLOCKLANGTH*2;
    ResUlst = Memory::resultData["Gorizontal"];
    ResUlstY = Memory::resultData["Vertical"];
    if(ResUlst.size() != sizeResp){
        ResUlst.clear();
        ResUlst.resize(sizeResp);
    }
    if(ResUlstY.size() != sizeResp){
        ResUlstY.clear();
        ResUlstY.resize(sizeResp);
    }
    int step = 0;
    int lastStep = 0;
    lastProgress = 0;
    for(int iNum = 0; iNum < Size; iNum++){
        float *DataBuf;
        DataBuf = dataFloat.data()+iNum*BLOCKRANGE;
        Math1(BLOCKRANGE,DataBuf);
        float index = 0;
        for(int i=0; i<BLOCKLANGTH; i++)
        {
               ResUlst[step] = ResXXAbs[i];
               ResUlstY[step] = ResYYAbs[i];
               if (maxColor < ResXXAbs[i])
                   maxColor = ResXXAbs[i];
               if (maxColor < ResYYAbs[i])
                   maxColor = ResYYAbs[i];
               ++step;
               // !!! значения аргумента в диапазоне от -180 до 180
               index = round(ResXXAng[i]+180); // округление аргумента для определения индекса ячейки
               // приведение фазы к диапазону от 0 до 360
               while(index >= 360) index -= 360;
               while(index < 0) index += 360;
               // накопление значений
               ResUlst[step] = index;
               // !!! значения аргумента в диапазоне от -180 до 180
               index = round(ResYYAng[i]+180); // округление аргумента для определения индекса ячейки
               // приведение фазы к диапазону от 0 до 360
               while(index >= 360) index -= 360;
               while(index < 0) index += 360;
               // накопление значений
               ResUlstY[step] = index;
               ++step;
        }
        compile(lastStep,step,iNum);
        lastStep = step;
    }
    emit progress(100);
    Memory::resultData["Gorizontal"] = ResUlst;
    Memory::resultData["Vertical"] = ResUlstY;
    emit resultGorizontal(histGA,histGY);
    emit resultVertical(histVA,histVY);
    clear();
}
void Worker::initPulse(int leSubBufNum, double leFreq){
    unsigned int BufSize = leSubBufNum*BLOCKLANGTH;
    memset(OriginalPulseRe,0,BLOCKLANGTH*sizeof(float));
    memset(OriginalPulseIm,0,BLOCKLANGTH*sizeof(float));
    for(unsigned int i=1; i < BufSize;i++)
    {
           if(i < 10)
           {
               float re = 128*cos(2*M_PI*i/Fd*leFreq*1e6);
               OriginalPulseRe[i] =re;
               float im = 128*sin(2*M_PI*i/Fd*leFreq*1e6);
               OriginalPulseIm[i] = im;
           }
    }
}
void Worker::clear(){
        delete[] ResXXRe;
        delete[] ResXXIm;
        delete[] ResYYRe;
        delete[] ResYYIm;

        delete[] ResXXAbs;
        delete[] ResXXAng;
        delete[] ResYYAbs;
        delete[] ResYYAng;

        delete[] ResXXPhase;
        delete[] ResYYPhase;


        delete[] a0XXsv0;
        delete[] a0XXsv1;
        delete[] a1XXsv0;
        delete[] a1XXsv1;
        delete[] a0YYsv0;
        delete[] a0YYsv1;
        delete[] a1YYsv0;
        delete[] a1YYsv1;
}
void Worker::Buffer(){
    ResXXRe = new float[BLOCKLANGTH];
    ResXXIm = new float[BLOCKLANGTH];
    ResYYRe = new float[BLOCKLANGTH];
    ResYYIm = new float[BLOCKLANGTH];

    ResXXAbs = new float[BLOCKLANGTH];
    ResXXAng = new float[BLOCKLANGTH];
    ResYYAbs = new float[BLOCKLANGTH];
    ResYYAng = new float[BLOCKLANGTH];

    ResXXPhase = new float[BLOCKLANGTH];
    ResYYPhase = new float[BLOCKLANGTH];

    a0XXsv0 = new float[BLOCKLANGTH];
    a0XXsv1 = new float[BLOCKLANGTH];
    a1XXsv0 = new float[BLOCKLANGTH];
    a1XXsv1 = new float[BLOCKLANGTH];
    a0YYsv0 = new float[BLOCKLANGTH];
    a0YYsv1 = new float[BLOCKLANGTH];
    a1YYsv0 = new float[BLOCKLANGTH];
    a1YYsv1 = new float[BLOCKLANGTH];
}
void Worker::MyCorrelation(float* in, int dataSize, float* kernel, int kernelSize, float* out)
{
        int i, j, k;

        // check validity of params
        if(!in || !out || !kernel) return;
        if(dataSize <=0 || kernelSize <= 0) return;

        // start convolution from out[kernelSize-1] to out[dataSize-1] (last)
        for(i = kernelSize-1; i < dataSize; ++i)
        {
                out[i] = 0;                             // init to 0 before accumulate

                for(j = i, k = 0; k < kernelSize; --j, ++k)
                        out[i] += in[j] * kernel[k];
        }

        // convolution from out[0] to out[kernelSize-2]
        for(i = 0; i < kernelSize - 1; ++i)
        {
                out[i] = 0;                             // init to 0 before sum

                for(j = i, k = 0; j >= 0; --j, ++k)
                        out[i] += in[j] * kernel[k];
        }

        return;
}
void Worker::Math1(unsigned int BufSize, float *DataBuf)
{
     float *a0XX, *a1XX, *a0YY, *a1YY;

     unsigned int Size = BufSize/4;

     a0XX = DataBuf+Size*0;
     a1XX = DataBuf+Size*1;
     a0YY = DataBuf+Size*2;
     a1YY = DataBuf+Size*3;


     MyCorrelation(a0XX, Size, OriginalPulseRe, Size, a0XXsv0);
     MyCorrelation(a0XX, Size, OriginalPulseIm, Size, a0XXsv1);
     MyCorrelation(a1XX, Size, OriginalPulseRe, Size, a1XXsv0);
     MyCorrelation(a1XX, Size, OriginalPulseIm, Size, a1XXsv1);
     MyCorrelation(a0YY, Size, OriginalPulseRe, Size, a0YYsv0);
     MyCorrelation(a0YY, Size, OriginalPulseIm, Size, a0YYsv1);
     MyCorrelation(a1YY, Size, OriginalPulseRe, Size, a1YYsv0);
     MyCorrelation(a1YY, Size, OriginalPulseIm, Size, a1YYsv1);

     for(unsigned int i=0; i<Size; i++)
     {
            // произведение сигналов с двух антенн в режиме ХХ с комплексным сопряжением
                        // Изменил знаки, как в YY, стало лучше, но сдвиг углов между гор. и верт. поляризац. около 3 град.
            ResXXRe[i] = (a0XXsv0[i]*a1XXsv0[i] + a0XXsv1[i]*a1XXsv1[i])/1152/8;//*cos(M_PI/2);

                        // 	ResXXRe[i] = -(a0XXsv0[i]*a1XXsv0[i] + a0XXsv1[i]*a1XXsv1[i])/1152/8;//*cos(M_PI/2);
            // изменён порядок вычисления нумерации антенн 2015.01.31
            ResXXIm[i] = (a0XXsv0[i]*a1XXsv1[i] - a0XXsv1[i]*a1XXsv0[i])/1152/8;//*sin(M_PI/2);
                       // - заменил на +
                       // ResXXIm[i] = (-a0XXsv0[i]*a1XXsv1[i] - a0XXsv1[i]*a1XXsv0[i])/1152/8;//*sin(M_PI/2);
            // вычисление фазы сигнала с антенны 0 в режиме ХХ
            if((a0XXsv1[i] != 0) && (a0XXsv0[i] != 0))

                 ResXXPhase[i] = 180*(atan2(a0XXsv1[i], a0XXsv0[i])+M_PI)/M_PI;
                                 //Убрал из   ResXXPhase[i]    +M_PI для устранения фазового сдвига между каналами - не помогло
                                // ResXXPhase[i] = 180*(atan2(a0XXsv1[i], a0XXsv0[i]))/M_PI;

            if((a0YYsv1[i] != 0) && (a0YYsv0[i] != 0))
                 ResYYPhase[i] = 180*(atan2(a0YYsv1[i], a0YYsv0[i])+M_PI)/M_PI;

            // вычисление модуля и аргумента произведения сигналов в режиме ХХ
            ResXXAbs[i] = pow(ResXXRe[i]*ResXXRe[i] + ResXXIm[i]*ResXXIm[i], 0.5);
            if(ResXXAbs[i] >=1e-13) ResXXAng[i] = RAD*(atan2(ResXXIm[i], ResXXRe[i]));
            else ResXXAng[i] = 0; // аргумент от -180 до 180 градусов

            // произведение сигналов с двух антенн в режиме YY с комплексным сопряжением
            // изменён порядок вычисления нумерации антенн 2015.01.31
            // !!!!! УБРАНА ИНВЕРСИЯ ПРОИЗВЕДЕНИЯ СИГНАЛОВ АНТЕНН ДЛЯ УСТРАНЕНИЯ СИСТЕМАТИЧЕСКОЙ
            // ПОГРЕШНОСТИ РАЗНОСТИ ФАЗ В 180 ГРАДУСОВ
            ResYYRe[i] = (a0YYsv0[i]*a1YYsv0[i] + a0YYsv1[i]*a1YYsv1[i])/1152/8;
            ResYYIm[i] = (a0YYsv0[i]*a1YYsv1[i] - a0YYsv1[i]*a1YYsv0[i])/1152/8;

            // вычисление модуля и аргумента произведения сигналов в режиме YY
            ResYYAbs[i] = pow(ResYYRe[i]*ResYYRe[i] + ResYYIm[i]*ResYYIm[i], 0.5);
            if(ResYYAbs[i] >= 1e-13) ResYYAng[i] = RAD*(atan2(ResYYIm[i], ResYYRe[i]));
            else ResYYAng[i] = 0;

     }
}
void Worker::sendMsgSlot(unsigned short BufferSize, unsigned char *Buffer, unsigned short CmdNum){
    SendCmdPacket(BufferSize,Buffer,CmdNum);
}
void Worker::sendParam(){
    unsigned short BuffSize = 70;
    double leFreq     = Memory::get("leFreq",0.0).toDouble(),
           leAmp      = Memory::get("leAmp",0.0).toDouble(),
           lePeriod   = Memory::get("lePeriod",0.0).toDouble(),
           leDuration = Memory::get("leDuration",0.0).toDouble(),
           leFreqRange= Memory::get("leFreqRange",0.0).toDouble(),
           leGateDelay= Memory::get("leGateDelay",0.0).toDouble(),
           leGateDuration = Memory::get("leGateDuration",0.0).toDouble();
    int    leBurstLen = Memory::get("leBurstLen",1).toInt();


    leAmp = leAmp > 0 ? ((leAmp/100)*0xFFF8)-1 : 0;
    QByteArray bleFreq;
    bleFreq.resize(BuffSize);
    unsigned char *DataPtr = (unsigned char *)bleFreq.data();
    memcpy(DataPtr,&leFreq,sizeof(double));
    DataPtr += sizeof(double);//8
    memcpy(DataPtr,&leAmp,sizeof(double));
    DataPtr += sizeof(double);//16
    memcpy(DataPtr,&lePeriod,sizeof(double));
    DataPtr += sizeof(double);//24
    memcpy(DataPtr,&leDuration,sizeof(double));
    DataPtr += sizeof(double);//32
    memcpy(DataPtr,&leBurstLen,sizeof(int));
    DataPtr += sizeof(int);//36
    memcpy(DataPtr,&leFreqRange,sizeof(double));
    DataPtr += sizeof(double);//44
    memcpy(DataPtr,&leGateDelay,sizeof(double));
    DataPtr += sizeof(double);//52
//    memcpy(DataPtr,&leGateDelay,sizeof(double));
    memcpy(DataPtr,&leGateDuration,sizeof(double));
    DataPtr += sizeof(double);//60
    bool cbLOGM,rbDdsRstBurst,cbMGEN,cbDDSReset,cbCont,cbUWB,cbGate,cbLFM,cbPulseMod;
    cbLOGM = Memory::get("cbLOGM",false).toBool();
    rbDdsRstBurst = Memory::get("rbDdsRstBurst",false).toBool();
    cbMGEN = Memory::get("cbMGEN",false).toBool();
    cbDDSReset = Memory::get("cbDDSReset",false).toBool();
    cbCont = Memory::get("cbCont",false).toBool();
    cbUWB = Memory::get("cbUWB",false).toBool();
    cbGate = Memory::get("cbGate",false).toBool();
    cbLFM = Memory::get("cbLFM",false).toBool();
    cbPulseMod = Memory::get("cbPulseMod",false).toBool();
    *((int *)(DataPtr)) = (((cbLOGM)?2:((rbDdsRstBurst)?0:1))<<23)|
            ((cbMGEN)<<9)|
            ((cbDDSReset)<<6)|
            ((cbCont)<<4)|
            ((cbUWB)<<3)|
            ((cbGate)<<2)|
            ((cbLFM)<<1)|
            ((cbPulseMod)<<0);
    DataPtr += sizeof(int);//64
    *((short *)(DataPtr)) = (short)Memory::get("leTxAtt",0).toInt()*2;
    DataPtr += sizeof(short);//66
    *((short *)(DataPtr)) = (short)Memory::get("leRxAtt",0).toInt()*2;
    DataPtr += sizeof(short);//68

    short TxPolDec =  (Memory::get("rbTxPolYY",false).toBool()<<3)|(Memory::get("rbTxPolYX",false).toBool()<<2)|
                                        (Memory::get("rbTxPolXY",false).toBool()<<1)|(Memory::get("rbTxPolXX",false).toBool()<<0);
    short RxPolDec =  (Memory::get("rbRxPolYY",false).toBool()<<3)|(Memory::get("rbRxPolYX",false).toBool()<<2)|
                                        (Memory::get("rbRxPolXY",false).toBool()<<1)|(Memory::get("rbRxPolXX",false).toBool()<<0);
    short RxAntDec =  (Memory::get("rbRxAnt1",false).toBool()<<1) |(Memory::get("rbRxAnt0",false).toBool()<<0);

    short TxPol = 0, RxPol = 0, RxAnt = 0;

    switch(TxPolDec)
    {
           case 0x1:{TxPol = 0;} break;
           case 0x2:{TxPol = 1;} break;
           case 0x4:{TxPol = 2;} break;
           case 0x8:{TxPol = 3;} break;
    }

    switch(RxPolDec)
    {
           case 0x1:{RxPol = 0;} break;
           case 0x2:{RxPol = 1;} break;
           case 0x4:{RxPol = 2;} break;
           case 0x8:{RxPol = 3;} break;
    }

    switch(RxAntDec)
    {
           case 0x1:{RxAnt = 0;} break;
           case 0x2:{RxAnt = 1;} break;
    }

    *((short *)(DataPtr)) = (RxAnt<<4)|(RxPol<<2)|(TxPol<<0);
    DataPtr += sizeof(short);//70

    *((short *)(DataPtr)) = (short)Memory::get("seMLEN",0).toInt();
    DataPtr += sizeof(short);//72
    SendCmdPacket(BuffSize, (unsigned char *)bleFreq.data(), 0x0001);
    //delete [] Buffer;
}
void Worker::sendMsg(QString info,QString address, quint16 port){
    QByteArray Datagram;
    QDataStream out(&Datagram, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_4);
        out << info;
        p_udpSocket->writeDatagram(info.toLatin1(), QHostAddress(address), port);
}
void Worker::sendMsg(QByteArray info,QString address, quint16 port){
    for(auto dl = info.begin(); dl!=info.end();++dl){
        unsigned int bData = (unsigned char)*dl;
        cout << bData << " " << std::ends;
    }
    cout << "" << endl;
    p_udpSocket->writeDatagram(info, QHostAddress(address), port);
}
void Worker::SendCmdPacket(unsigned short BufferSize, unsigned char *Buffer, unsigned short CmdNum){
    ++PacketNum;
    unsigned char *TxBuffer = new unsigned char[10];
    unsigned char *DataPtrB = TxBuffer;
    *(unsigned int   *)(DataPtrB) = 0xABCDDCBA;
    DataPtrB += sizeof(unsigned int);
     *(unsigned short *)(DataPtrB) = CmdNum;
    DataPtrB += sizeof(unsigned short);
    *(unsigned int   *)(DataPtrB) = PacketNum;
    QByteArray ab;
    ab.resize(10+BufferSize);
    unsigned char *DataPtr = (unsigned char *)ab.data();
    memcpy(DataPtr,TxBuffer,10);
    DataPtr+=10;
    memcpy(DataPtr,Buffer,BufferSize);
    QString address = Memory::get("rlsIP","127.0.0.1").toString();
    cout << "send: "<< address.toStdString() << endl;
    sendMsg(ab,address,Memory::get("rlsPort",30583).toInt());
    packet.insert(PacketNum,CmdNum);
}
