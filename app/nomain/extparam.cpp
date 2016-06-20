#include "extparam.h"
#include "ui_extparam.h"
#include "memory.h"
#include <QDesktopWidget>

ExtParam::ExtParam(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExtParam)
{
    ui->setupUi(this);
    //this->setWindowFlags(Qt::WindowStaysOnTopHint);
    ui->sArg->setValue(Memory::get("ArgMax",1024).toInt() - Memory::get("ArgMin",0).toInt());
    valArg = ui->sArg->value();
    valAng = 1;
    if(valArg > 1){
        valAng = normal/valArg;
        ui->sAng->setValue(valAng);
    }
    normal = 1;
    QDesktopWidget *desktop = QApplication::desktop();

    int screenWidth = desktop->width();
    int screenHeight = desktop->height();
    int width = this->geometry().width();
    int height = this->geometry().height();
    this->move((screenWidth-width)/2,(screenHeight-height)/2);
}

ExtParam::~ExtParam()
{
    delete ui;
}
void ExtParam::setData(QString name, QByteArray &dataArray){
    fname = name;
    tmpData = dataArray;
    normal = tmpData.size()/8;
    ui->sArg->setValue(Memory::get("ArgMax",1024).toInt() - Memory::get("ArgMin",0).toInt());
    valArg = ui->sArg->value();
    valAng = 1;
    if(valArg > 1){
        valAng = normal/valArg;
        ui->sAng->setValue(valAng);
    }
}

void ExtParam::on_upload_clicked()
{
    emit load(fname,tmpData);
    this->hide();
}

void ExtParam::on_sArg_valueChanged(int arg1)
{
    valArg = arg1;
    if(valArg > 1){
        valAng = normal/valArg;
        ui->sAng->setValue(valAng);
        Memory::set("ArgMin",0);
        Memory::set("ArgMax",valArg);
        emit sync();
    }
}

void ExtParam::on_sAng_valueChanged(int arg1)
{
    valAng = arg1;
    if(valAng > 1){
        valArg = normal/valAng;
        ui->sArg->setValue(valArg);
        Memory::set("ArgMin",0);
        Memory::set("ArgMax",valArg);
        emit sync();
    }
}
