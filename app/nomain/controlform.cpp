#include "controlform.h"
#include "ui_controlform.h"
#include "memory.h"
#include <iostream>

using namespace std;

ControlForm::ControlForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlForm)
{
    ui->setupUi(this);
    ui->leNumberOfMultOsc->setValue(Memory::get("leNumberOfMultOsc",0).toInt());
    ui->leMulOscDelay->setValue(Memory::get("leMulOscDelay",0).toInt());
    ui->leRasterPeriod->setValue(Memory::get("leRasterPeriod",0).toInt());
    ui->leScanSector->setValue(Memory::get("leScanSector",0.0).toDouble());
}

ControlForm::~ControlForm()
{
    delete ui;
}

void ControlForm::on_leNumberOfMultOsc_valueChanged(int arg1)
{
    Memory::set("leNumberOfMultOsc",arg1);
    Memory::set("Size",arg1);
    emit sync();
}

void ControlForm::on_leMulOscDelay_valueChanged(int arg1)
{
    Memory::set("leMulOscDelay",arg1);
    emit sync();
}

void ControlForm::on_leRasterPeriod_valueChanged(int arg1)
{
    Memory::set("leRasterPeriod",arg1);
    emit sync();
}

void ControlForm::on_leScanSector_valueChanged(double arg1)
{
    Memory::set("leScanSector",arg1);
    emit sync();
}

void ControlForm::on_bContView32Avg_clicked()
{
    unsigned short BuffSize = 4*sizeof(int);
    unsigned char *Buffer = new unsigned char[BuffSize];
    unsigned char *DataPtr = Buffer;

    int leSubBufNum = Memory::get("leSubBufNum",0).toInt()*sizeof(int);
    int leNumberOfMultOsc = Memory::get("leNumberOfMultOsc",0).toInt();
    int leMulOscDelay = Memory::get("leMulOscDelay",0).toInt();
    cout << "leSubBufNum: " << leSubBufNum << endl;
    cout << "leNumberOfMultOsc: " << leNumberOfMultOsc << endl;
    cout << "leMulOscDelay: " << leMulOscDelay << endl;

    memcpy(DataPtr,&leSubBufNum,sizeof(int));
    DataPtr += sizeof(int);
    *((unsigned int *)(DataPtr)) = 32;
    DataPtr += sizeof(int);
    memcpy(DataPtr,&leNumberOfMultOsc,sizeof(int));
    DataPtr += sizeof(int);
    memcpy(DataPtr,&leMulOscDelay,sizeof(int));
    DataPtr += sizeof(int);
    emit sendMsg(BuffSize, Buffer, 0x0007);
    cout << "Buffer:" << Buffer << endl;
//    delete [] Buffer;

}
