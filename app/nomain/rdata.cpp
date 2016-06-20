#include "rdata.h"

#include <QMutexLocker>

Rdata::Rdata()
{
    qm["dGorizontalAr"] = new QMutex();
    qm["dGorizontalPh"] = new QMutex();
    qm["dVerticalAr"] = new QMutex();
    qm["dVerticalPh"] = new QMutex();
    qm["vGorizontalAr"] = new QMutex();
    qm["vGorizontalPh"] = new QMutex();
    qm["vVerticalAr"] = new QMutex();
    qm["vVerticalPh"] = new QMutex();
    qm["srcData"] = new QMutex();
    qb.insert("dGorizontalAr",QByteArray());
    qb.insert("dGorizontalPh",QByteArray());
    qb.insert("dVerticalAr",QByteArray());
    qb.insert("dVerticalPh",QByteArray());
    qb.insert("vGorizontalAr",QByteArray());
    qb.insert("vGorizontalPh",QByteArray());
    qb.insert("vVerticalAr",QByteArray());
    qb.insert("vVerticalPh",QByteArray());
    qb.insert("srcData",QByteArray());
    qm["Var"] = new QMutex();
}
Rdata::~Rdata(){
    qm["dGorizontalAr"]->unlock();
    qm["dGorizontalPh"]->unlock();
    qm["dVerticalAr"]->unlock();
    qm["dVerticalPh"]->unlock();
    qm["vGorizontalAr"]->unlock();
    qm["vGorizontalPh"]->unlock();
    qm["vVerticalAr"]->unlock();
    qm["vVerticalPh"]->unlock();
}
QVariant Rdata::getVariant(QString name, QVariant defval){
    QMutexLocker locker(qm["Var"]);
    if(qv.contains(name))
        return qv[name];
    qv[name] = defval;
    return defval;
}
void Rdata::setVariant(QString name, QVariant val){
    qm["Var"]->lock();
    qv[name] = val;
    list[name] = val;
    qm["Var"]->unlock();
}
void Rdata::set(QString name, void *from, int length, unsigned int start){
    char *r;
    char *lsk = (char *)from;
    r = lsk+start;
    qm[name]->lock();
    if (qb[name].length() < length){
        qb[name].clear();
        qb[name].resize(length);
    }
    memcpy(qb[name].data(),r,length);
    qm[name]->unlock();
}
void Rdata::get(QString name, void *to, int length, unsigned int start){
    void *r;
    char *s = (char *)to;
    qm[name]->lock();
    if(qb[name].length()==0){
        qm[name]->unlock();
        return;
    }
    r = qb[name].data()+start;
    int position = length;
    int subP = 0;
    unsigned int lengthQb = qb[name].length();
    if(lengthQb < (unsigned int)length+start){
        position = qb[name].length() - start;
        subP = length-position;
    }
    memcpy(to,r,position);
    if(subP > 0)
        memset(s+position,0,subP);
    qm[name]->unlock();
}
