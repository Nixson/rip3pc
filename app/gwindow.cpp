#include <iostream>
#include "gwindow.h"
#include "ui_gwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QTimer>
#include <QTabWidget>
#include "nomain/extparam.h"
#include "pcap.h"

GWindow::GWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GWindow),
    control(new MControl)
{
    ui->setupUi(this);
    //определение серевых устройств
    QStringList devices;
    pcap_if_t *devlist;
    pcap_if_t *dev;
    char errbuf[PCAP_ERRBUF_SIZE];
    if(pcap_findalldevs(&devlist,errbuf) != -1){
        for(dev=devlist;dev;dev=dev->next)
            devices << QString::fromLocal8Bit(dev->name);
    }

    QList<QAction *> naList;
    bool checkedAction = false;
    QActionGroup *naGroup = new QActionGroup(this);
    foreach(QString dev, devices){
        QAction *na = new QAction(dev);
        na->setCheckable(true);
        if(Memory::get("device","").toString()==dev){
            checkedAction = true;
            na->setChecked(true);
        }
        else
            na->setChecked(false);
        connect(na,&QAction::triggered,[=](bool type){
            Q_UNUSED(type);
            control->setDev(dev);
        });
        naList.append(na);
        naGroup->addAction(na);
    }
    ui->devInterface->addActions(naList);
    if(!checkedAction && naList.size() > 0){
        naList[0]->setChecked(true);
    }


    //Настройка основных виджетов
    pb = new QProgressBar();
    pb->setMaximum(100);
    pb->setMinimum(0);
    pb->setTextVisible(true);
    logLabel = new QLabel();
    infoLabel = new QLabel();
    ui->statusBar->addWidget(logLabel,1);
    ui->statusBar->addWidget(infoLabel,1);
    ui->statusBar->addWidget(pb,1);
    pb->hide();
//    ui->statusBar->setLayoutDirection(Qt::LayoutDirection);
    this->setDockNestingEnabled(false);
    this->setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowTabbedDocks | QMainWindow::ForceTabbedDocks);
    ui->dockVisual->hide();
    control->setMDI(ui->mdiArea);
    connect(control,&MControl::setProgress,this,&GWindow::progress);
    connect(control,&MControl::setLog,this,&GWindow::log);
    connect(control,&MControl::loadPlugin,this,&GWindow::loadPlugin);
    connect(control,&MControl::hidePlugin,this,&GWindow::hidePlugin);
    control->load();
    debug = control->debug;
    addDock("debug","Панель отладки");
    QActionGroup* group = new QActionGroup( this );
    ui->mmGpolarization->setCheckable(true);
    ui->mmVpolarization->setCheckable(true);
    ui->mmGpolarization->setActionGroup(group);
    ui->mmVpolarization->setActionGroup(group);
    ui->mmGpolarization->setChecked(Memory::get("Gpolarization",true).toBool());
    ui->mmVpolarization->setChecked(Memory::get("Vpolarization",false).toBool());

    fs = new formSettings;
    connect(fs,&formSettings::save,control,&MControl::saveConfig);
    connect(fs,&formSettings::sendParam,control,&MControl::sendParam);
    connect(fs,&formSettings::sendMsg,control,&MControl::sendMsg);
    fs->setleSubBufNum(Memory::get("leSubBufNum",4).toInt());
    fs->setleFreq(Memory::get("leFreq",1777).toInt());
    fs->setleBurstLen(Memory::get("leBurstLen",1).toInt());
    fs->setleBurstLen(Memory::get("leBurstLen",1).toInt());
    fs->setleGeterodin(Memory::get("leGeterodin",8000).toInt());
    fs->setleAmp(Memory::get("leAmp",100).toInt());
    fs->setleFreqRange(Memory::get("leFreqRange",100).toInt());
    fs->setleGateDelay(Memory::get("leGateDelay",0).toInt());
    fs->setleGateDuration(Memory::get("leGateDuration",0.01).toDouble());
    fs->setcbPulseMod(Memory::get("cbPulseMod",true).toBool());
    fs->setcbUWB(Memory::get("cbUWB",false).toBool());
    fs->setcbLFM(Memory::get("cbLFM",false).toBool());
    fs->setlePeriod(Memory::get("lePeriod",0.1).toDouble());
    fs->setleDuration(Memory::get("leDuration",0.0033).toDouble());
    fs->setcbGate(Memory::get("cbGate",false).toBool());
    fs->setcbCont(Memory::get("cbCont",false).toBool());
    fs->setcbCoherentAccum(Memory::get("cbCoherentAccum",false).toBool());
    fs->setcbDDSReset(Memory::get("cbDDSReset",true).toBool());
    fs->setcbLOGM(Memory::get("cbLOGM",false).toBool());
    fs->setrbDdsRstBurst(Memory::get("rbDdsRstBurst",false).toBool());
    fs->setrbDdsRstPulse(Memory::get("rbDdsRstPulse",true).toBool());

    fs->setrbTxPolXX(Memory::get("rbTxPolXX",true).toBool());
    fs->setrbTxPolXY(Memory::get("rbTxPolXY",false).toBool());
    fs->setrbTxPolYX(Memory::get("rbTxPolYX",false).toBool());
    fs->setrbTxPolYY(Memory::get("rbTxPolYY",false).toBool());
    fs->setrbRxPolXX(Memory::get("rbRxPolXX",false).toBool());
    fs->setrbRxPolXY(Memory::get("rbRxPolXY",false).toBool());
    fs->setrbRxPolYX(Memory::get("rbRxPolYX",false).toBool());
    fs->setrbRxPolYY(Memory::get("rbRxPolYY",false).toBool());

    fs->setleTxAtt(Memory::get("leTxAtt",1).toInt());
    fs->setrlsIP(Memory::get("rlsIP","192.168.114.100").toString());
    fs->setrlsPort(Memory::get("rlsPort",30583).toInt());

    fs->setcbMGEN(Memory::get("cbMGEN",false).toBool());
    fs->setseMLEN(Memory::get("seMLEN",3).toInt());
    fs->setleRxAtt(Memory::get("leRxAtt",1).toInt());
    fs->setrbRxAnt0(Memory::get("rbRxAnt0",false).toBool());
    fs->setrbRxAnt1(Memory::get("rbRxAnt1",false).toBool());


    //Добавление дополнительных виджетов (окон)
    dw["fs"] = new QDockWidget("Настройка сканирования",this);
    dw["fs"]->setFloating(true);
    dw["fs"]->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //this->addDockWidget(Qt::RightDockWidgetArea,dw["fs"]);
    dw["fs"]->setWidget(fs);
    this->setDockNestingEnabled(false);
    dw["fs"]->hide();
    this->setTabPosition(Qt::RightDockWidgetArea,QTabWidget::East);
    this->setTabPosition(Qt::LeftDockWidgetArea,QTabWidget::West);
   // setTabPosition(Qt::RightDockWidgetArea,QTabWidget::TabPosition::East);
    //setTabPosition(Qt::LeftDockWidgetArea,QTabWidget::TabPosition::West);


    ui->Barier->setValue(Memory::get("Barier",0).toInt());
    ui->PhMin->setValue(Memory::get("PhMin",0).toInt());
    ui->ArgMax->setValue(Memory::get("ArgMax",1024).toInt());
    ui->ArgMin->setValue(Memory::get("ArgMin",0).toInt());
    this->showMaximized();
    sf = new SaveFile();
    connect(sf,&SaveFile::sync,control,&MControl::saveConfig);
    sf->hide();
    cf = new ControlForm;
    connect(cf,&ControlForm::sync,control,&MControl::saveConfig);
    connect(cf,&ControlForm::sendMsg,control,&MControl::sendMsg);
    dw["cf"] = new QDockWidget("Запрос",this);
    dw["cf"]->setFloating(true);
    dw["cf"]->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dw["cf"]->setWidget(cf);
    this->setDockNestingEnabled(false);
    dw["cf"]->hide();


    pl = new PolarisationGet;
    connect(pl,&PolarisationGet::sync,control,&MControl::saveConfig);
    dw["pl"] = new QDockWidget("Настройка поляризации",this);
    dw["pl"]->setFloating(true);
    dw["pl"]->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //this->addDockWidget(Qt::RightDockWidgetArea,dw["fs"]);
    dw["pl"]->setWidget(pl);
    this->setDockNestingEnabled(false);
    dw["pl"]->hide();


}

GWindow::~GWindow()
{
    delete ui;
    delete control;
}
/*
 * Загрузка сторонних плагинов
*/
void GWindow::loadPlugin(Plugs &plugin){
    if(plugins.contains(plugin.alias)){
        plugActions[plugin.alias]->setVisible(true);
        return;
    }
    QPluginLoader loader(plugin.path);
    if(loader.load()){
        plugins[plugin.alias] = plugin;
        PlugWidgetInterfaces *pluginI = qobject_cast<PlugWidgetInterfaces *>(loader.instance());
        plugObject[plugin.alias] = pluginI;
        dw[plugin.alias] = new QDockWidget(plugin.name,this);
        dw[plugin.alias]->setFloating(true);
        dw[plugin.alias]->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        dw[plugin.alias]->setWidget(pluginI);
        this->setDockNestingEnabled(false);
        dw[plugin.alias]->hide();

        QAction *pAction = ui->menuPlugins->addAction(plugin.name);
        plugActions[plugin.alias] = pAction;
        connect(pAction,&QAction::triggered,dw[plugin.alias],&QDockWidget::show);
        plugActions[plugin.alias]->setVisible(true);

        connect(pluginI,SIGNAL(processStep(int)),control,SLOT(shared(int)));
        connect(pluginI,SIGNAL(sync()),control,SLOT(saveConfig()));
        connect(control,SIGNAL(sync()),pluginI,SLOT(syncSlot()));
        pluginI->setMemory(Memory::dLink);
        pluginI->init();
    }
}
/*
 * Скрытие неиспользуемых плагинов
*/
void GWindow::hidePlugin(Plugs &plugin){
    if(plugins.contains(plugin.alias))
        plugActions[plugin.alias]->setVisible(false);
    else {
        loadPlugin(plugin);
//        plugActions[plugin.alias]->setVisible(false);
    }
}
/*
 * Отображение отладочной информации в панели отладки
*/
void GWindow::log(QString msg){
    logLabel->setText(msg);
}
/*
 * Конец загрузки файла или сканирования
*/
void GWindow::progressTimerNum(){
    pb->setValue(0);
    pb->hide();
}
/*
 * Метод, отвечающий за синхронизацию настроек и значений в виджете
*/
void GWindow::sync(){
    ui->PhMin->setValue(Memory::get("PhMin",0).toInt());
    ui->ArgMax->setValue(Memory::get("ArgMax",1024).toInt());
    ui->ArgMin->setValue(Memory::get("ArgMin",0).toInt());
    control->debug->log("ArgMax: "+QString::number(ui->ArgMax->value()));
    control->debug->log("ArgMin: "+QString::number(ui->ArgMin->value()));
}
/*
 * Метод, отображающий изменения в прогресс-баре
*/
void GWindow::progress(int num){
    if(!pb->isVisible())
        pb->show();
    pb->setValue(num);
    if(num==100){
        QTimer::singleShot(500, this, SLOT(progressTimerNum()));
    }
}
/*
 * Метод, с помощью которого добавляются виджеты в основное окно
*/
void GWindow::addDock(QString name, QString title){
    dw[name] = new QDockWidget(title,this);
    dw[name]->setFloating(true);
    dw[name]->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //this->addDockWidget(Qt::RightDockWidgetArea,dw[name]);
    if(name=="debug"){
        dw[name]->setWidget(debug);
    }
    this->setDockNestingEnabled(false);
    dw[name]->hide();
}
/*
 * Событие нажатия кнопки "Замостить"
*/
void GWindow::on_mmTile_triggered()
{
    this->ui->mdiArea->tileSubWindows();
}

/*
 * Событие нажатия кнопки "3D модель"
*/
void GWindow::on_mm3Dmodel_triggered()
{
    if(Memory::get("Vpolarization",true).toBool())
        control->showGr3D("vertical");
    else
        control->showGr3D("gorizontal");
}
/*
 * Событие нажатия кнопки "Горизонтальная поляризация"
*/

void GWindow::on_mmGpolarization_triggered(bool checked)
{
    control->debug->log("Gpolarization:"+QString::number(checked));
    control->setAction("Gpolarization",checked);
    control->setAction("Vpolarization",!checked);
    control->debug->log("Gpolarization:"+QString::number(Memory::get("Gpolarization",false).toBool()));
}
/*
 * Событие нажатия кнопки "Вертикальная поляризация"
*/

void GWindow::on_mmVpolarization_triggered(bool checked)
{
    control->debug->log("Vpolarization:"+QString::number(checked));
    control->setAction("Gpolarization",!checked);
    control->setAction("Vpolarization",checked);
    control->debug->log("Vpolarization:"+QString::number(Memory::get("Vpolarization",false).toBool()));
}
/*
 * Событие нажатия кнопки "Панель отладки"
 * Визуализируется виджет отладки
*/

void GWindow::on_mmDebug_triggered()
{
    dw["debug"]->show();
    this->setDockNestingEnabled(false);
}
/*
 * Событие нажатия кнопки "Панель отладки"
 * Открывается окно управления плагинами
*/

void GWindow::on_mmPlugs_triggered()
{
    control->winOpen("plugin");
}

/*
 * Событие нажатия кнопки "Настройки сигнала"
 * Открывается окно-виджет настроек РЛМ
*/
void GWindow::on_mmSignal_triggered()
{
    dw["fs"]->show();
    this->setDockNestingEnabled(false);
}

/*
 * Событие нажатия кнопки "Загрузить" из меню "Файл"
 * Открывается окно выбора файла.
*/
void GWindow::on_mmLoad_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Файл 3d объекта"), Memory::get("lastFileDir","").toString(),
            tr("Бинарные файлы (*.dat);;Бинарные файлы (*.3d);;Бинарные файлы (*.3df);;Все файлы (*.*)"));
    if (fileName != "") {   //Если файл выбран, то определяем расширение
            control->log("Загрузка файла");
            QFile file(fileName);
            Memory::set("lastFileDir",fileName);
            control->saveConfig();
            if (!file.open(QIODevice::ReadOnly)) {
                QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
                return;
            }
            QStringList fNameSpl = fileName.split(".");
            QString ext = fNameSpl[fNameSpl.size()-1];
            control->debug->log("ext: "+ext);
            QByteArray data = file.readAll();
            if(ext=="3d"){  //если 3d, тогда вызываем окно выбора размера кадра
                ExtParam *ex = new ExtParam;
                ex->setData(fileName,data);
                ex->show();
                // После определения размеров вызываем Worker::loadFinished
                connect(ex,&ExtParam::load,control,&MControl::loadData);
                connect(ex,&ExtParam::sync,this,&GWindow::sync);
                connect(ex,&ExtParam::log,control,&MControl::log);
                return;
            }
            // Если dat, вызываем Worker::loadSrc или Worker::loadFinishedF
            emit log(QString::number(data.size()));
            control->loadData(fileName,data);
        }
}
/*
 * Событие изменения максимальной дальности
 * Сохраняем новое значение и распространяем событие
*/

void GWindow::on_ArgMax_valueChanged(int value)
{
    Memory::set("ArgMax",value);
    control->saveConfig();
}

/*
 * Событие изменения минимальное дальности
 * Сохраняем новое значение и распространяем событие
*/
void GWindow::on_ArgMin_valueChanged(int value)
{
    Memory::set("ArgMin",value);
    control->saveConfig();
}

/*
 * Событие изменения угла
 * Сохраняем новое значение и распространяем событие
*/
void GWindow::on_PhMin_valueChanged(int value)
{
    Memory::set("PhMin",value);
    control->saveConfig();
}

/*
 * Событие изменения Порога отображения
 * Сохраняем новое значение и распространяем событие
*/
void GWindow::on_Barier_valueChanged(int value)
{
    Memory::set("Barier",value);
    control->saveConfig();
}
/*
 * Событие нажания кнопки "Растр"
 * Открывает окно растрового графика
*/

void GWindow::on_mmRastr_triggered()
{
    if(Memory::get("Vpolarization",true).toBool())
        control->showPlotRastr("vertical");
    else
        control->showPlotRastr("gorizontal");
}
/*
 * Событие нажания кнопки "АФ Растр"
 * Открывает окно графика АФ Растра
*/

void GWindow::on_mmAFRastr_triggered()
{
    if(Memory::get("Vpolarization",true).toBool())
        control->showPlotPolarization("vertical");
    else
        control->showPlotPolarization("gorizontal");
}
/*
 * Событие нажания кнопки "Сохранить"
 * Открывает окно сохрания резутьтата в файл
*/


void GWindow::on_mmSave_triggered()
{
    sf->show();
}
/*
 * Событие нажания кнопки "Непрерывный запрос"
 * Открывает окно запуска режима работы "Непрерывный запрос"
*/

void GWindow::on_mmControlForm_triggered()
{
    dw["cf"]->show();
}
/*
 * Событие нажания кнопки "Осцилограмма"
 * Открывает окно графика Осцилограмма
*/

void GWindow::on_mmOsc_triggered()
{
    if(Memory::get("Vpolarization",true).toBool())
        control->showPlotOsc("vertical");
    else
        control->showPlotOsc("gorizontal");
}
/*
 * Событие нажания кнопки "Настройка поляризации"
 * Открывает окно настроек поляризации
*/

void GWindow::on_mmPolarization_triggered()
{
    dw["pl"]->show();
}
