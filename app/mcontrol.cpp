#include "mcontrol.h"
#include "memory.h"
#include <QDockWidget>

MControl::MControl(QObject *parent) : QObject(parent), debug(new DebugDialog),
  isgr3dVmdi(false), isgr3dGmdi(false), isgrPlotVmdi(false), isgrPlotGmdi(false), isgrRPlotVmdi(false), isgrRPlotGmdi(false)
{
    settings = new QSettings("rip3p.ini",QSettings::IniFormat);
    Memory::resultData["Gorizontal"].clear();
    Memory::resultData["Vertical"].clear();
    QStringList keys = settings->childKeys();
    foreach (QString key, keys) {
         Memory::set(key,settings->value(key));
    }

    /*Memory::set("Gpolarization",settings->value("Gpolarization",true));
    Memory::set("Vpolarization",settings->value("Vpolarization",false));
    Memory::set("Size",settings->value("Size",0));
    Memory::set("Barier",settings->value("Barier",0));


    Memory::set("xRotation",settings->value("xRotation",0));
    Memory::set("yRotation",settings->value("yRotation",0));
    Memory::set("zRotation",settings->value("zRotation",0));
    Memory::set("VerticalScale",settings->value("VerticalScale",0));
    Memory::set("VerticalSLideX",settings->value("VerticalSLideX",0));
    Memory::set("VerticalSLideY",settings->value("VerticalSLideY",0));
    Memory::set("VerticalSLideZ",settings->value("VerticalSLideZ",0));
    Memory::set("GorizontalScale",settings->value("GorizontalScale",0));
    Memory::set("GorizontalSLideX",settings->value("GorizontalSLideX",0));
    Memory::set("GorizontalSLideY",settings->value("GorizontalSLideY",0));
    Memory::set("GorizontalSLideZ",settings->value("GorizontalSLideZ",0));

    Memory::set("leSubBufNum",settings->value("leSubBufNum",4));
    Memory::set("leFreq",settings->value("leFreq",1777));
    Memory::set("MaxColor",settings->value("MaxColor",QUINT16_SIZE));
    Memory::set("colorValue",settings->value("colorValue",0));
    Memory::set("ArgMin",settings->value("ArgMin",0));
    Memory::set("ArgMax",settings->value("ArgMax",1024));
    Memory::set("PhMin",settings->value("PhMin",0));
    Memory::set("PhMax",settings->value("PhMax",180));
    Memory::set("leBurstLen",settings->value("leBurstLen",1));
    Memory::set("leGeterodin",settings->value("leGeterodin",8000));
    Memory::set("leAmp",settings->value("leAmp",100));
    Memory::set("leFreqRange",settings->value("leFreqRange",100));
    Memory::set("leGateDelay",settings->value("leGateDelay",0));
    Memory::set("leGateDuration",settings->value("leGateDuration",0.01));
    Memory::set("cbPulseMod",settings->value("cbPulseMod",true));
    Memory::set("cbUWB",settings->value("cbUWB",false));
    Memory::set("cbLFM",settings->value("cbLFM",false));
    Memory::set("lePeriod",settings->value("lePeriod",0.1));
    Memory::set("leDuration",settings->value("leDuration",0.0033));
    Memory::set("cbGate",settings->value("cbGate",false));
    Memory::set("cbCont",settings->value("cbCont",false));
    Memory::set("cbCoherentAccum",settings->value("cbCoherentAccum",false));
    Memory::set("cbDDSReset",settings->value("cbDDSReset",true));
    Memory::set("cbLOGM",settings->value("cbLOGM",false));
    Memory::set("rbDdsRstBurst",settings->value("rbDdsRstBurst",false));
    Memory::set("rbDdsRstPulse",settings->value("rbDdsRstPulse",true));
    Memory::set("rbTxPolXX",settings->value("rbTxPolXX",true));
    Memory::set("rbTxPolXY",settings->value("rbTxPolXY",false));
    Memory::set("rbTxPolYX",settings->value("rbTxPolYX",false));
    Memory::set("rbTxPolYY",settings->value("rbTxPolYY",false));
    Memory::set("leTxAtt",settings->value("leTxAtt",1));
    Memory::set("plugins",settings->value("plugins",""));
    Memory::set("lastFileDir",settings->value("lastFileDir",""));
    Memory::set("lastPlugDir",settings->value("lastPlugDir",""));*/



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
void MControl::sendParam(){
    emit sendParamSignals();
}
void MControl::setDev(QString dev){
    Memory::set("device",dev);
    saveConfigTimer();
}
void MControl::sendMsg(unsigned short BufferSize, unsigned char *Buffer, unsigned short CmdNum){
    emit sendMsgSignal(BufferSize,Buffer,CmdNum);
}
void MControl::load(){
    plugin = new PlugWin;
    connect(plugin,&PlugWin::sync,this,&MControl::saveConfig);
    connect(plugin,&PlugWin::loadPlugin,this,&MControl::loadPlugin);
    connect(plugin,&PlugWin::hidePlugin,this,&MControl::hidePlugin);
    plugin->load();
}
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
void MControl::saveConfig(){
    if(saveTimer->isActive()){
        saveTimer->stop();
    }
    saveTimer->start();
    //Memory::scL
}
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
void MControl::progress(int vl){
    emit setProgress(vl);
    if(isgr3dVmdi)
        gr3dV->updates();
    if(isgr3dGmdi)
        gr3dG->updates();
}

void MControl::init(){

}
