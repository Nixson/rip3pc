#include "player.h"
#include "ui_player.h"

Player::Player() :
    ui(new Ui::Player)
{
    ui->setupUi(this);
    mainTimer = new QTimer();
    connect(mainTimer,&QTimer::timeout,this,&Player::nextStep);
    speed = 100;
    width = 10;
    Size = 400;
}
void Player::setMemory(Rdata *mLink){
    mem = mLink;
}
void Player::init(){
    speed = mem->getVariant("plugPlayerSpeed",100).toInt();
    width = mem->getVariant("plugPlayerWidth",10).toInt();
    Size = mem->getVariant("Size",400).toInt();
    ui->speed->setValue(speed);
    ui->screen->setValue(width);
    ui->screen->setMaximum(Size);
    mainTimer->setInterval(1+10000/speed);
}
Player::~Player()
{
    delete ui;
}
void Player::syncSlot(){
    speed = mem->getVariant("plugPlayerSpeed",100).toInt();
    width = mem->getVariant("plugPlayerWidth",10).toInt();
    Size = mem->getVariant("Size",400).toInt();
    ui->speed->setValue(speed);
    ui->screen->setValue(width);
    ui->screen->setMaximum(Size);
}

void Player::on_speed_valueChanged(int arg1)
{
    mem->setVariant("plugPlayerSpeed",arg1);
    speed = arg1;
    mainTimer->setInterval(1+10000/speed);
    emit sync();
}

void Player::on_screen_valueChanged(int arg1)
{
    mem->setVariant("plugPlayerWidth",arg1);
    width = arg1;
    emit sync();
}
void Player::nextStep(){
    if(speed >= 100){
        Position+=speed/100;
    }else
        ++Position;
    if(Position >= Size){
        on_start_clicked();
        return;
    }
    vGA.fill(0.0);
    vGP.fill(0.0);
    vVA.fill(0.0);
    vVP.fill(0.0);
    int start = Position - width;
    if(start < 0)
        start = 0;
    int range = (Position - start)*BLOCKLANGTH*sizeof(float);
    start *=BLOCKLANGTH;
    int length = Size*BLOCKLANGTH*sizeof(float);
    float *vGAl = vGA.data();
    float *GAl = GA.data();
    memcpy(vGAl+start,GAl+start,range);
    float *vGPl = vGP.data();
    float *GPl = GP.data();
    memcpy(vGPl+start,GPl+start,range);
    float *vVAl = vVA.data();
    float *VAl = VA.data();
    memcpy(vVAl+start,VAl+start,range);
    float *vVPl = vVP.data();
    float *VPl = VP.data();
    memcpy(vVPl+start,VPl+start,range);


    mem->set("vGorizontalAr",vGA.data(),length);
    mem->set("vGorizontalPh",vGP.data(),length);
    mem->set("vVerticalAr",vVA.data(),length);
    mem->set("vVerticalPh",vVP.data(),length);
    emit processStep(Position);
}

void Player::on_pause_clicked()
{
    if(mainTimer->isActive()){
        mainTimer->stop();
        ui->pause->setText("Возобновить");
    }
    else {
        mainTimer->start();
        ui->pause->setText("Пауза");
    }
}

void Player::on_start_clicked()
{
    ui->pause->setText("Пауза");
    if(mainTimer->isActive()){
        mainTimer->stop();
        ui->start->setText("Старт");
        Position = Size;
    }
    else {
        Position = 0;
        GA.clear();
        GP.clear();
        VA.clear();
        VP.clear();
        GA.resize(Size*BLOCKLANGTH);
        GP.resize(Size*BLOCKLANGTH);
        VA.resize(Size*BLOCKLANGTH);
        VP.resize(Size*BLOCKLANGTH);
        GA.fill(0.0);
        GP.fill(0.0);
        VA.fill(0.0);
        VP.fill(0.0);
        int length = Size*BLOCKLANGTH*sizeof(float);
        mem->get("dGorizontalAr",GA.data(),length);
        mem->get("dGorizontalPh",GP.data(),length);
        mem->get("dVerticalAr",VA.data(),length);
        mem->get("dVerticalPh",VP.data(),length);//range,start
        ui->start->setText("Стоп");
        vGA.clear();
        vGP.clear();
        vVA.clear();
        vVP.clear();
        vGA.resize(Size*BLOCKLANGTH);
        vGP.resize(Size*BLOCKLANGTH);
        vVA.resize(Size*BLOCKLANGTH);
        vVP.resize(Size*BLOCKLANGTH);
        vGA.fill(0.0);
        vGP.fill(0.0);
        vVA.fill(0.0);
        vVP.fill(0.0);
        mem->set("vGorizontalAr",vGA.data(),length);
        mem->set("vGorizontalPh",vGP.data(),length);
        mem->set("vVerticalAr",vVA.data(),length);
        mem->set("vVerticalPh",vVP.data(),length);
        emit processStep(Size);
        mainTimer->start();
    }

}
