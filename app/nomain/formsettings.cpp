#include "formsettings.h"
#include "ui_formsettings.h"
#include "memory.h"
#include <QDesktopWidget>

formSettings::formSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::formSettings)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    QDesktopWidget *desktop = QApplication::desktop();
    int screenWidth = desktop->width();
    int screenHeight = desktop->height();
    int width = this->geometry().width();
    int height = this->geometry().height();
    this->move((screenWidth-width)/2,(screenHeight-height)/2);
}

formSettings::~formSettings()
{
    delete ui;
}

void formSettings::on_leSubBufNum_valueChanged(int arg1)
{
    Memory::set("leSubBufNum",arg1);
    emit leSubBufNum(arg1);
    emit save();
}

void formSettings::on_leBurstLen_valueChanged(int arg1)
{
    Memory::set("leBurstLen",arg1);
    emit leBurstLen(arg1);
    emit save();
}

void formSettings::on_leGeterodin_valueChanged(int arg1)
{
    Memory::set("leGeterodin",arg1);
    emit leGeterodin(arg1);
    emit save();
}

void formSettings::on_leFreq_valueChanged(int arg1)
{
    Memory::set("leFreq",arg1);
    emit leFreq(arg1);
    emit save();
}

void formSettings::on_leAmp_valueChanged(int arg1)
{
    Memory::set("leAmp",arg1);
    emit leAmp(arg1);
    emit save();
}

void formSettings::on_cbPulseMod_toggled(bool checked)
{
    Memory::set("cbPulseMod",checked);
    emit cbPulseMod(checked);
    emit save();
}

void formSettings::on_cbUWB_toggled(bool checked)
{
    Memory::set("cbUWB",checked);
    emit cbUWB(checked);
    emit save();
}

void formSettings::on_cbLFM_toggled(bool checked)
{
    Memory::set("cbLFM",checked);
    emit cbLFM(checked);
    emit save();
}

void formSettings::on_lePeriod_valueChanged(double arg1)
{
    Memory::set("lePeriod",arg1);
    emit lePeriod(arg1);
    emit save();
}

void formSettings::on_leDuration_valueChanged(double arg1)
{
    Memory::set("leDuration",arg1);
    emit leDuration(arg1);
    emit save();
}

void formSettings::on_leFreqRange_valueChanged(int arg1)
{
    Memory::set("leFreqRange",arg1);
    emit leFreqRange(arg1);
    emit save();
}

void formSettings::on_cbGate_toggled(bool checked)
{
    Memory::set("cbGate",checked);
    emit cbGate(checked);
    emit save();
}

void formSettings::on_leGateDelay_valueChanged(int arg1)
{
    Memory::set("leGateDelay",arg1);
    emit leGateDelay(arg1);
    emit save();
}

void formSettings::on_leGateDuration_valueChanged(double arg1)
{
    Memory::set("leGateDuration",arg1);
    emit leGateDuration(arg1);
    emit save();
}

void formSettings::on_cbCont_toggled(bool checked)
{
    Memory::set("cbCont",checked);
    emit cbCont(checked);
    emit save();
}

void formSettings::on_cbCoherentAccum_toggled(bool checked)
{
    Memory::set("cbCoherentAccum",checked);
    emit cbCoherentAccum(checked);
    emit save();
}

void formSettings::on_cbDDSReset_toggled(bool checked)
{
    Memory::set("cbDDSReset",checked);
    emit cbDDSReset(checked);
    emit save();
}

void formSettings::on_cbLOGM_toggled(bool checked)
{
    Memory::set("cbLOGM",checked);
    emit cbLOGM(checked);
    emit save();
}

void formSettings::on_rbDdsRstBurst_toggled(bool checked)
{
    Memory::set("rbDdsRstBurst",checked);
    emit rbDdsRstBurst(checked);
    emit save();
}

void formSettings::on_rbDdsRstPulse_toggled(bool checked)
{
    Memory::set("rbDdsRstPulse",checked);
    emit rbDdsRstPulse(checked);
    emit save();
}
void formSettings::setleSubBufNum(int value){
    ui->leSubBufNum->setValue(value);
}
void formSettings::setleBurstLen(int value){
    ui->leBurstLen->setValue(value);
}

void formSettings::setleGeterodin(int val){
    ui->leGeterodin->setValue(val);
}
void formSettings::setleFreq(int val){
    ui->leFreq->setValue(val);
}
void formSettings::setleAmp(int val){
    ui->leAmp->setValue(val);
}
void formSettings::setcbPulseMod(bool val){
    ui->cbPulseMod->setChecked(val);
}
void formSettings::setcbUWB(bool val){
    ui->cbUWB->setChecked(val);
}
void formSettings::setcbLFM(bool val){
    ui->cbLFM->setChecked(val);
}
void formSettings::setlePeriod(double val){
    ui->lePeriod->setValue(val);
}
void formSettings::setleDuration(double val){
    ui->leDuration->setValue(val);
}
void formSettings::setleFreqRange(int val){
    ui->leFreqRange->setValue(val);
}
void formSettings::setcbGate(bool val){
    ui->cbGate->setChecked(val);
}
void formSettings::setleGateDelay(int val){
    ui->leGateDelay->setValue(val);
}
void formSettings::setleGateDuration(double val){
    ui->leGateDuration->setValue(val);
}
void formSettings::setcbCont(bool val){
    ui->cbCont->setChecked(val);
}
void formSettings::setcbCoherentAccum(bool val){
    ui->cbCoherentAccum->setChecked(val);
}
void formSettings::setcbDDSReset(bool val){
    ui->cbDDSReset->setChecked(val);
}
void formSettings::setcbLOGM(bool val){
    ui->cbLOGM->setChecked(val);
}
void formSettings::setrbDdsRstBurst(bool val){
    ui->rbDdsRstBurst->setChecked(val);
}
void formSettings::setrbDdsRstPulse(bool val){
    ui->rbDdsRstPulse->setChecked(val);
}

void formSettings::setrbTxPolXX(bool val){
    ui->rbTxPolXX->setChecked(val);
}
void formSettings::setrbTxPolXY(bool val){
    ui->rbTxPolXY->setChecked(val);
}
void formSettings::setrbTxPolYX(bool val){
    ui->rbTxPolYX->setChecked(val);
}
void formSettings::setrbTxPolYY(bool val){
    ui->rbTxPolYY->setChecked(val);
}


void formSettings::setleTxAtt(int val){
    ui->leTxAtt->setValue(val);
}

void formSettings::on_rbTxPolXX_toggled(bool checked)
{
    Memory::set("rbTxPolXX",checked);
    emit save();
}

void formSettings::on_rbTxPolXY_toggled(bool checked)
{
    Memory::set("rbTxPolXY",checked);
    emit save();
}

void formSettings::on_rbTxPolYX_toggled(bool checked)
{
    Memory::set("rbTxPolYX",checked);
    emit save();
}

void formSettings::on_rbTxPolYY_toggled(bool checked)
{
    Memory::set("rbTxPolYY",checked);
    emit save();
}

void formSettings::on_leTxAtt_valueChanged(int arg1)
{
    Memory::set("leTxAtt",arg1);
    emit save();
}

void formSettings::on_BitBtn1_clicked()
{
    emit sendParam();
    QByteArray ba;
    ba.resize(sizeof(int));
    unsigned int leSubBufNum = (unsigned int)Memory::get("leSubBufNum",1).toInt();
    memcpy(ba.data(),&leSubBufNum,sizeof(int));
    QTimer::singleShot(1000,[=](){
        emit sendMsg(4, (unsigned char *)ba.data(), 0x0002);
    });

}

void formSettings::setrlsIP(QString val){
    ui->rlsIP->setText(val);
}
void formSettings::on_rlsIP_textEdited(const QString &arg1)
{
    Memory::set("rlsIP",arg1);
    emit save();
}

void formSettings::on_rlsIP_textChanged(const QString &arg1)
{
    Memory::set("rlsIP",arg1);
    emit save();
}
void formSettings::setrbRxAnt0(bool checked){
    ui->rbRxAnt0->setChecked(checked);
}
void formSettings::setrbRxAnt1(bool checked){
    ui->rbRxAnt1->setChecked(checked);
}
void formSettings::on_rbRxAnt0_toggled(bool checked)
{
    Memory::set("rbRxAnt0",checked);
    emit save();
}

void formSettings::on_rbRxAnt1_toggled(bool checked)
{
    Memory::set("rbRxAnt1",checked);
    emit save();
}
void formSettings::setcbMGEN(bool checked){
    ui->cbMGEN->setChecked(checked);
}
void formSettings::on_cbMGEN_toggled(bool checked)
{
    Memory::set("cbMGEN",checked);
    emit save();
}
void formSettings::setseMLEN(int arg1){
    ui->seMLEN->setValue(arg1);
}

void formSettings::on_seMLEN_valueChanged(int arg1)
{
    Memory::set("seMLEN",arg1);
    emit save();
}
void formSettings::setrlsPort(int port){
    ui->rlsPort->setValue(port);
}
void formSettings::on_rlsPort_valueChanged(int arg1)
{
    Memory::set("rlsPort",arg1);
    emit save();
}

void formSettings::on_leRxAtt_valueChanged(int arg1)
{
    Memory::set("leRxAtt",arg1);
    emit save();
}
void formSettings::setleRxAtt(int val){
    ui->leRxAtt->setValue(val);
}

void formSettings::on_rbRxPolXX_toggled(bool checked)
{
    Memory::set("rbRxPolXX",checked);
    emit save();
}

void formSettings::on_rbRxPolXY_toggled(bool checked)
{
    Memory::set("rbRxPolXY",checked);
    emit save();
}

void formSettings::on_rbRxPolYX_toggled(bool checked)
{
    Memory::set("rbRxPolYX",checked);
    emit save();
}

void formSettings::on_rbRxPolYY_toggled(bool checked)
{
    Memory::set("rbRxPolYY",checked);
    emit save();
}
void formSettings::setrbRxPolXX(bool checked){
    ui->rbRxPolXX->setChecked(checked);
}
void formSettings::setrbRxPolXY(bool checked){
    ui->rbRxPolXY->setChecked(checked);
}
void formSettings::setrbRxPolYX(bool checked){
    ui->rbRxPolYX->setChecked(checked);
}
void formSettings::setrbRxPolYY(bool checked){
    ui->rbRxPolYY->setChecked(checked);
}
