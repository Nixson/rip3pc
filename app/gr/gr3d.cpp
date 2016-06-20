#include "gr3d.h"
#include "ui_gr3d.h"
#include "memory.h"

Gr3D::Gr3D(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Gr3D)
{
    ui->setupUi(this);
    pgl = new PlotGl();
    connect(pgl,&PlotGl::xRotationChanged,this,&Gr3D::xRotationChanged);
    connect(pgl,&PlotGl::yRotationChanged,this,&Gr3D::yRotationChanged);
    connect(pgl,&PlotGl::zRotationChanged,this,&Gr3D::zRotationChanged);
}
void Gr3D::setType(QString typeName){
    pgl->setType(typeName);
    tName = typeName;
    ui->controlBlock->addWidget(pgl);
    ui->SlideScale->setValue(Memory::get(tName+"Scale",0).toInt());
    ui->SlideX->setValue(Memory::get(tName+"SLideX",0).toInt());
    ui->SlideY->setValue(Memory::get(tName+"SLideY",0).toInt());
    ui->SlideZ->setValue(Memory::get(tName+"SLideZ",0).toInt());
}

Gr3D::~Gr3D()
{
    delete ui;
}
void Gr3D::updates(){
    pgl->updateSc();
}

void Gr3D::on_SlideScale_valueChanged(int value)
{
    pgl->setScale(ui->SlideScale->maximum()-value);
    Memory::set(tName+"Scale",value);
    emit sync();
}

void Gr3D::on_SlideZ_valueChanged(int value)
{
    pgl->setZRotation(value);
    Memory::set(tName+"SLideZ",value);
    emit sync();
}

void Gr3D::on_SlideX_valueChanged(int value)
{
    pgl->setXRotation(value);
    Memory::set(tName+"SLideX",value);
    emit sync();
}

void Gr3D::on_SlideY_valueChanged(int value)
{
    pgl->setYRotation(value);
    Memory::set(tName+"SLideY",value);
    emit sync();
}
void Gr3D::xRotationChanged(int val){
    ui->SlideX->setValue(val);
    Memory::set(tName+"SLideX",val);
    emit sync();
}
void Gr3D::yRotationChanged(int val){
    ui->SlideY->setValue(val);
    Memory::set(tName+"SLideY",val);
    emit sync();
}
void Gr3D::zRotationChanged(int val){
    ui->SlideZ->setValue(val);
    Memory::set(tName+"SLideZ",val);
    emit sync();
}
