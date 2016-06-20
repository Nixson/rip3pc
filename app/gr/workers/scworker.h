#ifndef SCWORKER_H
#define SCWORKER_H

#include <QObject>
#include "typelist.h"

class ScWorker : public QObject
{
    Q_OBJECT
public:
    explicit ScWorker(QObject *parent = 0);

signals:
    void plot(Clowd &data);

public slots:
    void syncSlot();
    void dataSlot(Clowd &dataA, Clowd &dataH);
    void plotSlot();
    void sharedSlot(int ship, QString type);
private:
    int m_count, AngleMin, AngleMax, OffsetMin, OffsetMax, Size, maxColor;
    uint MaxBarier;
    GLfloat delY, centerX, centerY, centerZ;
    Clowd m_data, histA, histY;

    GLfloat *m_dataLink;
    GLfloat VertexArray[12][3];

    void plotFlower(int x,int y,int z, uint color);
    void appendPointObj(float x, float y, float z, int color);

};

#endif // SCWORKER_H
