#include "plugwin.h"
#include "ui_plugwin.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QCheckBox>
#include <QStyle>
#include <QDesktopWidget>
#include "memory.h"

PlugWin::PlugWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlugWin)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    this->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            this->size(),
            qApp->desktop()->availableGeometry()
        )
    );
    ui->tableWidget->setColumnCount(3);
    QHeaderView *header = new QHeaderView(Qt::Horizontal,ui->tableWidget);
    header->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->hide();
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Interactive);
    ui->tableWidget->horizontalHeader()->resizeSection(1, 30);
    ui->tableWidget->horizontalHeader()->hide();
}

PlugWin::~PlugWin()
{
    delete ui;
}

void PlugWin::on_addPlug_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Добавить плагин"), Memory::get("lastPlugDir","").toString(),
            tr("Файлы плагинов (*.so)"));
    if (fileName != "") {
        Memory::set("lastPlugDir",fileName);
        QPluginLoader loader(fileName);
        Plugs p;
        p.enable = true;
        p.name = loader.metaData().value("MetaData").toObject().value("Descr").toString();
        p.alias = loader.metaData().value("MetaData").toObject().value("Name").toString();
        p.path = fileName;
//        emit loadPlugin(p.name,alias,loader);
        QStringList plugs = Memory::get("plugins","").toStringList();
        plugs << p.path;
        Memory::set("plugins",plugs);
        emit sync();
        tableAppend(p);
    }

}
void PlugWin::load(){
    QStringList plugs = Memory::get("plugins","").toStringList();
    foreach (QString plugPath, plugs) {
        QFileInfo check_file(plugPath);
        if(check_file.exists()){
            QPluginLoader loader(plugPath);
            Plugs p;
            p.enable = true;
            p.name = loader.metaData().value("MetaData").toObject().value("Descr").toString();
            p.alias = loader.metaData().value("MetaData").toObject().value("Name").toString();
            p.path = plugPath;
            tableAppend(p);
        }
    }
}

void PlugWin::tableAppend(Plugs &p){
    QFileInfo check_file(p.path);
    if(check_file.exists()){
        plugList.insert(p.name,p);
        int rows = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(rows);
        QTableWidgetItem *item0 = new QTableWidgetItem();
        QTableWidgetItem *item1 = new QTableWidgetItem();
        QTableWidgetItem *item2 = new QTableWidgetItem();
        item0->setTextAlignment(Qt::AlignHCenter);
        /*if (p.enable)
            item0->setCheckState(Qt::Checked);
        else
            item0->setCheckState(Qt::Unchecked);*/
        item1->setText(p.name);
        item2->setText(p.path);
        ui->tableWidget->setItem(rows, 0, item0);
        ui->tableWidget->setItem(rows, 1, item1);
        ui->tableWidget->setItem(rows, 2, item2);




        QWidget *pWidget = new QWidget();
        QCheckBox *pCheckBox = new QCheckBox();
        QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
        pCheckBox->setChecked(p.enable);
//        connect(pCheckBox,&QCheckBox::toggled,this,&PlugWin::checked);
        //PlugWin *tw = this;
        connect(pCheckBox,&QCheckBox::toggled,[=](bool checked){this->checkedItem(item0,checked);});
        pLayout->addWidget(pCheckBox);
        pLayout->setAlignment(Qt::AlignCenter);
        pLayout->setContentsMargins(0,0,0,0);
        pWidget->setLayout(pLayout);
        ui->tableWidget->setCellWidget(0,0,pWidget);
        checkedItem(item0,p.enable);

        /*QCheckBox *chb = new QCheckBox("");
        chb->setChecked(p.enable);
        chb->setObjectName(p.name);
        ui->tableWidget->setCellWidget(rows,0,chb);*/
    }
}
void PlugWin::checked(bool check){
    bool status = check;
    if(status){}

}
void PlugWin::checkedItem(QTableWidgetItem *itm, bool check){
    int row = itm->row();
    QString name = ui->tableWidget->item(row,1)->text();
    bool status = check;
    if(plugList[name].enable!=status) {
        plugList[name].enable = status;
    }
    if(status){
        emit loadPlugin(plugList[name]);
    }
    else
        emit hidePlugin(plugList[name]);
    //bool status = itm->checkState();
}
