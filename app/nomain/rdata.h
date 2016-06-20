#ifndef RDATA_H
#define RDATA_H

#include <QMap>
#include <QMutex>
#include <QVariant>

class Rdata
{
public:
    explicit Rdata();
    ~Rdata();
    void set(QString name, void *from, int length, unsigned int start = 0);
    void get(QString name,void *to, int length, unsigned int start = 0);
    void setVariant(QString name,QVariant val);
    QVariant getVariant(QString name,QVariant defval);
    QMap<QString, QVariant> list;

private:
    QMap<QString, QMutex *> qm;
    QMap<QString, QByteArray> qb;
    QMap<QString, QVariant> qv;
};

#endif // RDATA_H
