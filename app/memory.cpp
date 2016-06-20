#include "memory.h"

Memory::Memory(){}
#ifndef MEMORY_INIT
#define MEMORY_INIT
QMap<QString,QVariant> Memory::info = QMap<QString,QVariant>();
QMap<QString,ScObject*> Memory::scL = QMap<QString,ScObject*>();
QMap<QString,Clowd> Memory::resultData = QMap<QString,Clowd>();
QMap<QString,fdata_t*> Memory::dataLink = QMap<QString,fdata_t*>();
Rdata *Memory::dLink = new Rdata();
#endif // MEMORY_INIT

void Memory::set(QString name, QVariant value){
    dLink->setVariant(name,value);
//    info[name] = value;
}
QVariant Memory::get(QString name, QVariant def){
    return dLink->getVariant(name,def);
/*    if(info.contains(name))
        return info[name];
    return def;
    */
}
void Memory::clearData(){
//    Memory::dLink->clear();
}
void Memory::setData(QString name, void *from, int length, unsigned int start){
    Memory::dLink->set(name, from, length, start);
}
void Memory::getData(QString name, void *to, int length, unsigned int start){
    Memory::dLink->get(name, to, length, start);
}
