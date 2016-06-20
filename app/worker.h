#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThreadPool>
#include "typelist.h"
#include "nomain/udpsock.h"
#include <QUdpSocket>


class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = 0);
    ~Worker();

signals:
    void save();
    void log(QString);
    void progress(int);
    void shared(int);
    void resultGorizontal(Clowd &dataArg,Clowd &dataPh);
    void resultVertical(Clowd &dataArg,Clowd &dataPh);
    void updateInterface();

public slots:
    void loadSrc(QByteArray &data);
    void loadFinished(QByteArray &data);
    void loadFinishedF(QByteArray &data);
    void sync();
    void sendParam();
    void sendMsgSlot(unsigned short BufferSize, unsigned char *Buffer, unsigned short CmdNum);
private:
    QByteArray data;
    MathVector dataDouble;
    Clowd dataFloat;

    float *OriginalPulseRe, *OriginalPulseIm;
    void initPulse(int leSubBufNum, double leFreq);
    void Buffer();
    void clear();
    void run();
    void Math1(unsigned int BufSize, float *DataBuf);
    void MyCorrelation(float *in, int dataSize, float *kernel, int kernelSize, float *out);
    void compile(int startPos, int position, int iNum);
    void attach();
    bool isAttach;


    float *a0XXsv0;
    float *a0XXsv1;
    float *a1XXsv0;
    float *a1XXsv1;
    float *a0YYsv0;
    float *a0YYsv1;
    float *a1YYsv0;
    float *a1YYsv1;

    float *ResXXRe, *ResXXIm;
    float *ResYYRe, *ResYYIm;
    float *ResXXAbs, *ResXXAng;
    float *ResYYAbs, *ResYYAng;
    float *ResXXPhase, *ResYYPhase;
    float *ResXXReAvg, *ResXXImAvg;
    float *ResYYReAvg, *ResYYImAvg;
    Clowd histGA,histGY,histVA,histVY;
    int lastProgress;
    float maxColor, colorStep;
    int Size;
    Clowd ResUlst, ResUlstY;
    int ArgMin, ArgMax;

    UDPSock *udp;
    QThreadPool *tp;

    QUdpSocket *p_udpSocket;
    int PacketNum;
    QMap <int, int> packet;
    void sendMsg(QString info, QString address, quint16 port);
    void sendMsg(QByteArray info, QString address, quint16 port);
    void SendCmdPacket(unsigned short BufferSize, unsigned char *Buffer, unsigned short CmdNum);

};

#endif // WORKER_H
