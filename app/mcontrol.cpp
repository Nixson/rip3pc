#include "mcontrol.h"
#include "memory.h"
#include <QDockWidget>
#include <iostream>

MControl::MControl(QObject *parent) : QObject(parent), debug(new DebugDialog),
  isgr3dVmdi(false), isgr3dGmdi(false), isgrPlotVmdi(false), isgrPlotGmdi(false), isgrRPlotVmdi(false), isgrRPlotGmdi(false)
{
    // получение ранее сохраненных настроек
    settings = new QSettings("rip3p.ini",QSettings::IniFormat);
    Memory::resultData["Gorizontal"].clear();
    Memory::resultData["Vertical"].clear();
    QStringList keys = settings->childKeys();
    foreach (QString key, keys) {
         Memory::set(key,settings->value(key));
    }
    // Запуск основного рабочего класса и перенаправление его в отдельный поток
    worker = new Worker;
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this,&MControl::loadSrc,worker,&Worker::loadSrc);
    connect(this,&MControl::loadFinished,worker,&Worker::loadFinished);
    connect(this,&MControl::loadFinishedF,worker,&Worker::loadFinishedF);
    connect(this,&MControl::sync,worker,&Worker::sync);
    connect(this,&MControl::sendParamSignals,worker,&Worker::sendParam);
    connect(this,&MControl::sendMsgSignal,worker,&Worker::sendMsgSlot);
    connect(worker,&Worker::save,this,&MControl::saveConfig);

    connect(worker,&Worker::log,this,&MControl::log);
    connect(worker,&Worker::progress,this,&MControl::progress);

    connect(worker,&Worker::shared,this,&MControl::shared);
    connect(worker,&Worker::resultGorizontal,this,&MControl::resultXX);
    connect(worker,&Worker::resultVertical,this,&MControl::resultYY);
    workerThread.start();
    hasGData = false;
    hasVData = false;
    saveTimer = new QTimer();
    saveTimer->setInterval(300);
    connect(saveTimer,&QTimer::timeout,this,&MControl::saveConfigTimer);
}
MControl::~MControl(){
    workerThread.quit();
    workerThread.wait();
}
/*
 * Прокси-метод для вызова метода Worker::sendParam
*/
void MControl::sendParam(){
    emit sendParamSignals();
}
/*
 * Метод-сигнал изменения сетевого устройства
*/
void MControl::setDev(QString dev){
    Memory::set("device",dev);
    saveConfigTimer();
}
/*
 * Прокси-метод, для вызова Worker::sendMsgSlot
*/
void MControl::sendMsg(unsigned short BufferSize, unsigned char *Buffer, unsigned short CmdNum){
    emit sendMsgSignal(BufferSize,Buffer,CmdNum);
}
/*
 * Создание окна управления плагинами
*/
void MControl::load(){
    plugin = new PlugWin;
    connect(plugin,&PlugWin::sync,this,&MControl::saveConfig);
    connect(plugin,&PlugWin::loadPlugin,this,&MControl::loadPlugin);
    connect(plugin,&PlugWin::hidePlugin,this,&MControl::hidePlugin);
    plugin->load();
}
/*
 * Синхронизация изменения настроек программы (срабатывает от таймера)
*/
void MControl::saveConfigTimer(){
    saveTimer->stop();
    QMap<QString, QVariant>::const_iterator i = Memory::dLink->list.constBegin();
    while (i != Memory::dLink->list.constEnd()) {
        settings->setValue(i.key(),i.value());
        ++i;
    }
    settings->sync();
    emit sync();
    if(isgr3dVmdi){
        Memory::scL["Vertical"]->sync();
    }
    if(isgr3dGmdi){
        Memory::scL["Gorizontal"]->sync();
    }
}
/*
 * Запуск таймера синхронизации. Если таймер работает, прекращаем работу и после этого запускаем
*/
void MControl::saveConfig(){
    if(saveTimer->isActive()){
        saveTimer->stop();
    }
    saveTimer->start();
    //Memory::scL
}
/*
 * Метод вызывается, когда файл полностью отработан. (горизонтальная поляризация)
 * Получает финальные данные и отдает из всем графикам
*/
void MControl::resultXX(Clowd &dataA,Clowd &dataH){
    int sizeA = dataA.size()*sizeof(float);
    int sizeH = dataH.size()*sizeof(float);
    Memory::setData("dGorizontalAr",dataA.data(),sizeA);
    Memory::setData("vGorizontalAr",dataA.data(),sizeA);
    Memory::setData("dGorizontalPh",dataH.data(),sizeH);
    Memory::setData("vGorizontalPh",dataH.data(),sizeH);
    hasGData = true;
    if(isgr3dGmdi){
        Memory::scL["Gorizontal"]->setBuf(dataA,dataH);
        gr3dG->pgl->stopTimer();
    }
    if(isgrPlotGmdi){
        grPlotG->setBuf(dataA,dataH);
        grPlotG->plot();
    }
    if(isgrRPlotGmdi){
        grRPlotG->setBuf(dataA,dataH);
        grRPlotG->plot();
    }
    if(isgrOPlotGmdi){
        grOPlotG->shared(Memory::get("Size",1).toInt());
    }
}
/*
 * Метод вызывается, когда файл полностью отработан. (вертикальная поляризация)
 * Получает финальные данные и отдает из всем графикам
*/
void MControl::resultYY(Clowd &dataA, Clowd &dataH){
    int sizeA = dataA.size()*sizeof(float);
    int sizeH = dataH.size()*sizeof(float);
    Memory::setData("dVerticalAr",dataA.data(),sizeA);
    Memory::setData("vVerticalAr",dataA.data(),sizeA);
    Memory::setData("dVerticalPh",dataH.data(),sizeH);
    Memory::setData("vVerticalPh",dataH.data(),sizeH);
    hasVData = true;
    if(isgr3dVmdi){
        Memory::scL["Vertical"]->setBuf(dataA,dataH);
        gr3dV->pgl->stopTimer();
    }
    if(isgrPlotVmdi){
        grPlotV->setBuf(dataA,dataH);
        grPlotV->plot();
    }
    if(isgrRPlotVmdi){
        grRPlotV->setBuf(dataA,dataH);
        grRPlotV->plot();
    }
    if(isgrOPlotVmdi){
        grOPlotV->shared(Memory::get("Size",1).toInt());
    }
}
/*
 * Обработка команды shared
 * Передает её всем активным графикам
*/
void MControl::shared(int shp){
    if(isgr3dVmdi){
        Memory::scL["Vertical"]->shared(shp);
        gr3dV->pgl->updateTimer();
    }
    if(isgr3dGmdi){
        Memory::scL["Gorizontal"]->shared(shp);
        gr3dG->pgl->updateTimer();
    }
    if(isgrPlotGmdi){
        grPlotG->shared(shp);
    }
    if(isgrPlotVmdi){
        grPlotV->shared(shp);
    }
    if(isgrRPlotGmdi){
        grRPlotG->shared(shp);
    }
    if(isgrRPlotVmdi){
        grRPlotV->shared(shp);
    }
    if(isgrOPlotGmdi){
        grOPlotG->shared(shp);
    }
    if(isgrOPlotVmdi){
        grOPlotV->shared(shp);
    }
}
/*
 * Метод по расширению файла определяет вызываемых обработчик
*/
void MControl::loadData(QString fileName, QByteArray &data){
    QStringList ex = fileName.split(".");
    QString extension = ex[ex.length()-1];
    if(extension=="dat"){
        emit loadSrc(data);
    }
    else if(extension=="3d"){
        emit loadFinished(data);
    }
    else if(extension=="3df"){
        emit loadFinishedF(data);
    }
}

void MControl::setMDI(QMdiArea *lnk){
    area = lnk;
}
/*
 * Открытие 3D графика
*/
void MControl::showGr3D(QString sType){
    if(sType=="vertical"){
        if(!isgr3dVmdi){
            isgr3dVmdi = true;
            gr3dV = new Gr3D();
            gr3dV->setType("Vertical");
            connect(gr3dV,&Gr3D::sync,this,&MControl::saveConfig);
            gr3dVmdi = area->addSubWindow(gr3dV);
            gr3dVmdi->setWindowTitle("3D Вертикальная поляризация");
            gr3dVmdi->resize(400,400);
            connect(gr3dVmdi,SIGNAL(destroyed()),this,SLOT(isgr3dVmdiHide()));
            if(hasVData){
                // Если данные уже есть, то отображаем
                Clowd bufA, bufP;
                int size = Memory::get("Size",1024).toInt()*BLOCKLANGTH;
                bufA.resize(size);
                bufP.resize(size);
                Memory::getData("vVerticalAr",bufA.data(),size*sizeof(float));
                Memory::getData("vVerticalPh",bufP.data(),size*sizeof(float));
                Memory::scL["Vertical"]->setBuf(bufA,bufP);
            }
        }
        gr3dVmdi->show();
    }
    else{
        if(!isgr3dGmdi){
            isgr3dGmdi = true;
            gr3dG = new Gr3D();
            gr3dG->setType("Gorizontal");
            connect(gr3dG,&Gr3D::sync,this,&MControl::saveConfig);
            gr3dGmdi = area->addSubWindow(gr3dG);
            gr3dGmdi->setWindowTitle("3D Горизонтальная поляризация");
            gr3dGmdi->resize(400,400);
            connect(gr3dGmdi,SIGNAL(destroyed()),this,SLOT(isgr3dGmdiHide()));
            if(hasGData){
                // Если данные уже есть, то отображаем
                Clowd bufA, bufP;
                int size = Memory::get("Size",1024).toInt()*BLOCKLANGTH;
                bufA.resize(size);
                bufP.resize(size);
                Memory::getData("vGorizontalAr",bufA.data(),size*sizeof(float));
                Memory::getData("vGorizontalPh",bufP.data(),size*sizeof(float));
                Memory::scL["Gorizontal"]->setBuf(bufA,bufP);
            }
        }
        gr3dGmdi->show();
    }
}
void MControl::showPlotRastr(QString sType){
    if(sType=="vertical"){
        if(!isgrRPlotVmdi){
            isgrRPlotVmdi = true;
            grRPlotV = new PlotRaster();
            grRPlotV->setType("Vertical");
            grRPlotV->syncSlot();
            connect(grRPlotV,&PlotRaster::sync,this,&MControl::saveConfig);
            connect(this,&MControl::sync,grRPlotV,&PlotRaster::syncSlot);
            grRPlotVmdi = area->addSubWindow(grRPlotV);
            grRPlotVmdi->setWindowTitle("Вертикальная поляризация");
            grRPlotVmdi->resize(400,400);
            connect(grRPlotVmdi,SIGNAL(destroyed()),this,SLOT(isgrRPlotVmdiHide()));
            if(hasVData){
                // Если данные уже есть, то отображаем
                Clowd bufA, bufP;
                int size = Memory::get("Size",1024).toInt()*BLOCKLANGTH;
                bufA.resize(size);
                bufP.resize(size);
                Memory::getData("vVerticalAr",bufA.data(),size*sizeof(float));
                Memory::getData("vVerticalPh",bufP.data(),size*sizeof(float));
                grRPlotV->setBuf(bufA,bufP);
                grRPlotV->plot();
            }
        }
        grRPlotVmdi->show();
    }else {
        if(!isgrRPlotGmdi){
            isgrRPlotGmdi = true;
            grRPlotG = new PlotRaster();
            grRPlotG->setType("Gorizontal");
            grRPlotG->syncSlot();
            connect(grRPlotG,&PlotRaster::sync,this,&MControl::saveConfig);
            connect(this,&MControl::sync,grRPlotG,&PlotRaster::syncSlot);
            grRPlotGmdi = area->addSubWindow(grRPlotG);
            grRPlotGmdi->setWindowTitle("Горизонтальная поляризация");
            grRPlotGmdi->resize(400,400);
            connect(grRPlotGmdi,SIGNAL(destroyed()),this,SLOT(isgrRPlotGmdiHide()));
            if(hasGData){
                // Если данные уже есть, то отображаем
                Clowd bufA, bufP;
                int size = Memory::get("Size",1024).toInt()*BLOCKLANGTH;
                bufA.resize(size);
                bufP.resize(size);
                Memory::getData("vGorizontalAr",bufA.data(),size*sizeof(float));
                Memory::getData("vGorizontalPh",bufP.data(),size*sizeof(float));
                grRPlotG->setBuf(bufA,bufP);
                grRPlotG->plot();
            }
        }
        grRPlotGmdi->show();
    }
}

void MControl::showPlotOsc(QString sType){
    if(sType=="vertical"){
        if(!isgrOPlotVmdi){
            isgrOPlotVmdi = true;
            grOPlotV = new PlotOsc();
            grOPlotV->setType("Vertical");
            grOPlotV->syncSlot();
            connect(grOPlotV,&PlotOsc::sync,this,&MControl::saveConfig);
            connect(this,&MControl::sync,grOPlotV,&PlotOsc::syncSlot);
            grOPlotVmdi = area->addSubWindow(grOPlotV);
            grOPlotVmdi->setWindowTitle("Вертикальная осцилограмма");
            grOPlotVmdi->resize(400,400);
            connect(grOPlotVmdi,SIGNAL(destroyed()),this,SLOT(isgrOPlotVmdiHide()));
            if(hasVData){
                // Если данные уже есть, то отображаем
                int size = Memory::get("Size",1024).toInt();
                grOPlotV->shared(size);
                grOPlotV->plot();
            }
        }
        grOPlotVmdi->show();
    }else {
        if(!isgrOPlotGmdi){
            isgrOPlotGmdi = true;
            grOPlotG = new PlotOsc();
            grOPlotG->setType("Gorizontal");
            grOPlotG->syncSlot();
            connect(grOPlotG,&PlotOsc::sync,this,&MControl::saveConfig);
            connect(this,&MControl::sync,grOPlotG,&PlotOsc::syncSlot);
            grOPlotGmdi = area->addSubWindow(grOPlotG);
            grOPlotGmdi->setWindowTitle("Горизонтальная осцилограмма");
            grOPlotGmdi->resize(400,400);
            connect(grOPlotGmdi,SIGNAL(destroyed()),this,SLOT(isgrOPlotGmdiHide()));
            if(hasGData){
                // Если данные уже есть, то отображаем
                int size = Memory::get("Size",1024).toInt();
                grOPlotG->shared(size);
                grOPlotG->plot();
            }
        }
        grOPlotGmdi->show();
    }
}
void MControl::showPlotPolarization(QString sType){
    if(sType=="vertical"){
        if(!isgrPlotVmdi){
            isgrPlotVmdi = true;
            grPlotV = new PlotPolarization();
            grPlotV->setType("Vertical");
            grPlotV->syncSlot();
            connect(grPlotV,&PlotPolarization::sync,this,&MControl::saveConfig);
            connect(this,&MControl::sync,grPlotV,&PlotPolarization::syncSlot);
            grPlotVmdi = area->addSubWindow(grPlotV);
            grPlotVmdi->setWindowTitle("Вертикальная поляризация");
            grPlotVmdi->resize(400,400);
            connect(grPlotVmdi,SIGNAL(destroyed()),this,SLOT(isgrPlotVmdiHide()));
            if(hasVData){
                // Если данные уже есть, то отображаем
                Clowd bufA, bufP;
                int size = Memory::get("Size",1024).toInt()*BLOCKLANGTH;
                bufA.resize(size);
                bufP.resize(size);
                Memory::getData("vVerticalAr",bufA.data(),size*sizeof(float));
                Memory::getData("vVerticalPh",bufP.data(),size*sizeof(float));
                grPlotV->setBuf(bufA,bufP);
                grPlotV->plot();
            }
        }
        grPlotVmdi->show();
    }
    else {
        if(!isgrPlotGmdi){
            isgrPlotGmdi = true;
            grPlotG = new PlotPolarization();
            grPlotG->setType("Gorizontal");
            grPlotG->syncSlot();
            connect(grPlotG,&PlotPolarization::sync,this,&MControl::saveConfig);
            connect(this,&MControl::sync,grPlotG,&PlotPolarization::syncSlot);
            grPlotGmdi = area->addSubWindow(grPlotG);
            grPlotGmdi->setWindowTitle("Горизонтальная поляризация");
            grPlotGmdi->resize(400,400);
            connect(grPlotGmdi,SIGNAL(destroyed()),this,SLOT(isgrPlotGmdiHide()));
            if(hasGData){
                // Если данные уже есть, то отображаем
                Clowd bufA, bufP;
                int size = Memory::get("Size",1024).toInt()*BLOCKLANGTH;
                bufA.resize(size);
                bufP.resize(size);
                Memory::getData("vGorizontalAr",bufA.data(),size*sizeof(float));
                Memory::getData("vGorizontalPh",bufP.data(),size*sizeof(float));
                grPlotG->setBuf(bufA,bufP);
                grPlotG->plot();
            }
        }
        grPlotGmdi->show();
    }
}

void MControl::isgrPlotVmdiHide(){
    isgrPlotVmdi = false;
}
void MControl::isgrPlotGmdiHide(){
    isgrPlotGmdi = false;
}
void MControl::isgr3dVmdiHide(){
    isgr3dVmdi = false;
}
void MControl::isgr3dGmdiHide(){
    isgr3dGmdi = false;
}
void MControl::isgrRPlotVmdiHide(){
    isgrRPlotVmdi = false;
}
void MControl::isgrRPlotGmdiHide(){
    isgrRPlotGmdi = false;
}
void MControl::isgrOPlotVmdiHide(){
    isgrOPlotVmdi = false;
}
void MControl::isgrOPlotGmdiHide(){
    isgrOPlotGmdi = false;
}
void MControl::winOpen(QString winName){
    if(winName=="plugin")
        plugin->show();

}
/*
 * Метод, для передачи настроек в систему сохранения
*/
void MControl::setAction(QString name, QVariant value){
    Memory::set(name,value);
    settings->setValue(name,value);
    settings->sync();
    debug->log(name+":"+QString::number(value.toBool()));
    debug->log(name+":"+QString::number(settings->value(name,false).toBool()));
}
void MControl::log(QString line){
    debug->log(line);
    emit setLog(line);
}
/*
 * Прокси-метод передачи прогресс-бару
*/
void MControl::progress(int vl){
    emit setProgress(vl);
    if(isgr3dVmdi)
        gr3dV->updates();
    if(isgr3dGmdi)
        gr3dG->updates();
}

void MControl::init(){

}
