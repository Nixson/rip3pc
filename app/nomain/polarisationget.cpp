#include "polarisationget.h"
#include "ui_polarisationget.h"
#include "memory.h"
#include <QDesktopWidget>

PolarisationGet::PolarisationGet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PolarisationGet)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    QDesktopWidget *desktop = QApplication::desktop();
    int screenWidth = desktop->width();
    int screenHeight = desktop->height();
    int width = this->geometry().width();
    int height = this->geometry().height();
    this->move((screenWidth-width)/2,(screenHeight-height)/2);
    ui->aPhi1->setValue(Memory::get("aPhi1",15).toInt());
    ui->aPhi2->setValue(Memory::get("aPhi2",31).toInt());
    ui->sPhi1->setValue(Memory::get("sPhi1",0).toInt());
    ui->sPhi2->setValue(Memory::get("sPhi2",0).toInt());
    ui->aPhi1_2->setValue(Memory::get("aPhi1v",0).toInt());
    ui->aPhi2_2->setValue(Memory::get("aPhi2v",16).toInt());
    ui->sPhi1_2->setValue(Memory::get("sPhi1v",0).toInt());
    ui->sPhi2_2->setValue(Memory::get("sPhi2v",0).toInt());
}

PolarisationGet::~PolarisationGet()
{
    delete ui;
}

void PolarisationGet::on_aPhi1_valueChanged(int arg1)
{
    Memory::set("aPhi1",arg1);
    emit sync();
}

void PolarisationGet::on_aPhi2_valueChanged(int arg1)
{
    Memory::set("aPhi2",arg1);
    emit sync();
}

void PolarisationGet::on_sPhi1_valueChanged(int arg1)
{
    Memory::set("sPhi1",arg1);
    emit sync();
}

void PolarisationGet::on_sPhi2_valueChanged(int arg1)
{
    Memory::set("sPhi2",arg1);
    emit sync();
}

void PolarisationGet::on_aPhi1_2_valueChanged(int arg1)
{
    Memory::set("aPhi1v",arg1);
    emit sync();
}

void PolarisationGet::on_aPhi2_2_valueChanged(int arg1)
{
    Memory::set("aPhi2v",arg1);
    emit sync();
}

void PolarisationGet::on_sPhi1_2_valueChanged(int arg1)
{
    Memory::set("sPhi1v",arg1);
    emit sync();
}

void PolarisationGet::on_sPhi2_2_valueChanged(int arg1)
{
    Memory::set("sPhi2v",arg1);
    emit sync();
}
