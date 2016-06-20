#include "savefile.h"
#include "ui_savefile.h"
#include "memory.h"
#include <QFileDialog>
#include <QDateTime>
#include <QDesktopWidget>

SaveFile::SaveFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SaveFile)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    QDesktopWidget *desktop = QApplication::desktop();
    int screenWidth = desktop->width();
    int screenHeight = desktop->height();
    int width = this->geometry().width();
    int height = this->geometry().height();
    this->move((screenWidth-width)/2,(screenHeight-height)/2);
    ui->folderPath->setText(Memory::get("safePath","").toString());
    ui->checkBox->setChecked(Memory::get("saveGorizontal",true).toBool());
    ui->checkBox_2->setChecked(Memory::get("saveVertical",true).toBool());
    ui->experimentName->setText(Memory::get("saveExperimentName","").toString());
}

SaveFile::~SaveFile()
{
    delete ui;
}

void SaveFile::on_checkBox_toggled(bool checked)
{
    Memory::set("saveGorizontal",checked);
    emit sync();
}

void SaveFile::on_checkBox_2_toggled(bool checked)
{
    Memory::set("saveVertical",checked);
    emit sync();
}

void SaveFile::on_editFolder_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Изменить папку назначения"),
                                                    ui->folderPath->text(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    ui->folderPath->setText(dir);
    Memory::set("safePath",dir);
    emit sync();
}

void SaveFile::on_saveButton_clicked()
{
    QDateTime td(QDateTime::currentDateTime());
    QString nowDt = td.toString("dd-MM-yyyy hh-mm-ss");
    QString name = ui->experimentName->text();
    int Size = Memory::get("Size",0).toInt()*BLOCKLANGTH*sizeof(float);
    if(Size==0)
        return;
    if(ui->checkBox->isChecked()){
        QString fname = ui->folderPath->text()+"/"+name+"_horizontal_"+nowDt+".3df";
        QFile file(fname);
        file.open(QIODevice::WriteOnly);
        QByteArray ba;
        ba.resize(Size);
        Memory::getData("vGorizontalAr",ba.data(),Size);
        file.write(ba);
        ba.clear();
        ba.resize(Size);
        Memory::getData("vGorizontalPh",ba.data(),Size);
        file.write(ba);
        file.close();
    }
    if(ui->checkBox_2->isChecked()){
        QString fname = ui->folderPath->text()+"/"+name+"_vertical_"+nowDt+".3df";
        QFile file(fname);
        file.open(QIODevice::WriteOnly);
        QByteArray ba;
        ba.resize(Size);
        Memory::getData("vVerticalAr",ba.data(),Size);
        file.write(ba);
        ba.clear();
        ba.resize(Size);
        Memory::getData("vVerticalPh",ba.data(),Size);
        file.write(ba);
        file.close();
    }
    this->hide();
}

void SaveFile::on_experimentName_textChanged(const QString &arg1)
{
    Memory::set("saveExperimentName",arg1);
    emit sync();
}

void SaveFile::on_experimentName_textEdited(const QString &arg1)
{
    Memory::set("saveExperimentName",arg1);
    emit sync();
}
