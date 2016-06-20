#ifndef SCOBJECT_H
#define SCOBJECT_H

#include <QObject>
#include <QThread>
#include "typelist.h"
#include <QTimer>
#include "gr/workers/scworker.h"

class ScObject : public QObject
{
    Q_OBJECT
    QThread mathThread;
public:
    explicit ScObject(QObject *parent = 0);
    ~ScObject();
    const GLfloat *constData() const { return m_data.constData(); }
    const GLfloat *constDataLines() const { return l_data.constData(); }
    int count() const { return m_count; }
    int countLines() const { return l_count; }
    int vertexCount() const { return m_count / 6; }
    int vertexCountLines() const { return l_count / 6; }
    void editData(const Clowd &result);
    void setMax(int);
    void setAngle(int,int);
    void setOffset(unsigned int, unsigned int);
    void setSizeBlock(unsigned int);
    void setType(QString typeName);
    void sync();
    Clowd c_data;
    void loadDataImage();
    void shared(int);
    void setBuf(Clowd &dataA, Clowd &dataH);

signals:
    void syncSignal();
    void dataSignal(Clowd &dataA, Clowd &dataH);
    void sharedSignal(int ship, QString type);
    void plotSignal();
    void plot();

public slots:
    void sharedNow();
    void result(Clowd &data);

private:
    Clowd m_data;
    Clowd l_data, buf;
    Clowd histA, histY;
    QString tName;
    int m_count;
    int l_count;
    int maxColor;
    int maxX;
    int maxY;
    int maxZ;
    void clearData();
    int AngleMin, AngleMax;
    unsigned int OffsetMin;
    unsigned int OffsetMax;
    unsigned int Size,MaxBarier;
    void reLine();
    GLfloat centerX, centerY, centerZ;
    void appendPoint(int x,int y,int z);
    GLfloat delY;
    GLfloat *sObjectLink, *m_dataLink;
    void appendPointObj(int x,int y,int z, int color);
    bool plotWork,rePlot;
    void plotFlower(int x,int y,int z, unsigned int color);
    bool isShared;
    QTimer *sharedTimer;
    int sharedShip;
    ScWorker *worker;


};

#endif // SCOBJECT_H
