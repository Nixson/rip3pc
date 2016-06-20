#ifndef FORMSETTINGS_H
#define FORMSETTINGS_H

#include <QDialog>

namespace Ui {
class formSettings;
}

class formSettings : public QDialog
{
    Q_OBJECT

public:
    explicit formSettings(QWidget *parent = 0);
    ~formSettings();

signals:
    void leSubBufNum(int);
    void leBurstLen(int);
    void leGeterodin(int);
    void leFreq(int);
    void leAmp(int);
    void cbPulseMod(bool);
    void cbUWB(bool);
    void cbLFM(bool);
    void lePeriod(double);
    void leDuration(double);
    void leFreqRange(int);
    void cbGate(bool);
    void leGateDelay(int);
    void leGateDuration(double);
    void cbCont(bool);
    void cbCoherentAccum(bool);
    void cbDDSReset(bool);
    void cbLOGM(bool);
    void rbDdsRstBurst(bool);
    void rbDdsRstPulse(bool);
    void save();
    void sendParam();
    void sendMsg(unsigned short BufferSize, unsigned char *Buffer, unsigned short CmdNum);


public slots:
    void setleSubBufNum(int);
    void setleBurstLen(int);
    void setleGeterodin(int);
    void setleFreq(int);
    void setleAmp(int);
    void setcbPulseMod(bool);
    void setcbUWB(bool);
    void setcbLFM(bool);
    void setlePeriod(double);
    void setleDuration(double);
    void setleFreqRange(int);
    void setcbGate(bool);
    void setleGateDelay(int);
    void setleGateDuration(double);
    void setcbCont(bool);
    void setcbCoherentAccum(bool);
    void setcbDDSReset(bool);
    void setcbLOGM(bool);
    void setrbDdsRstBurst(bool);
    void setrbDdsRstPulse(bool);
    void setrbTxPolXX(bool);
    void setrbTxPolXY(bool);
    void setrbTxPolYX(bool);
    void setrbTxPolYY(bool);
    void setleTxAtt(int);
    void setrlsIP(QString);
    void setrlsPort(int);
    void setrbRxAnt0(bool);
    void setrbRxAnt1(bool);
    void setseMLEN(int arg1);
    void setcbMGEN(bool checked);
    void setleRxAtt(int);

    void setrbRxPolXX(bool);
    void setrbRxPolXY(bool);
    void setrbRxPolYX(bool);
    void setrbRxPolYY(bool);

private slots:
    void on_leSubBufNum_valueChanged(int arg1);
    void on_leBurstLen_valueChanged(int arg1);
    void on_leGeterodin_valueChanged(int arg1);
    void on_leFreq_valueChanged(int arg1);
    void on_leAmp_valueChanged(int arg1);
    void on_cbPulseMod_toggled(bool checked);
    void on_cbUWB_toggled(bool checked);
    void on_cbLFM_toggled(bool checked);
    void on_lePeriod_valueChanged(double arg1);
    void on_leDuration_valueChanged(double arg1);
    void on_leFreqRange_valueChanged(int arg1);
    void on_cbGate_toggled(bool checked);
    void on_leGateDelay_valueChanged(int arg1);
    void on_leGateDuration_valueChanged(double arg1);
    void on_cbCont_toggled(bool checked);
    void on_cbCoherentAccum_toggled(bool checked);
    void on_cbDDSReset_toggled(bool checked);
    void on_cbLOGM_toggled(bool checked);
    void on_rbDdsRstBurst_toggled(bool checked);
    void on_rbDdsRstPulse_toggled(bool checked);
    void on_rbTxPolXX_toggled(bool checked);
    void on_rbTxPolXY_toggled(bool checked);
    void on_rbTxPolYX_toggled(bool checked);
    void on_rbTxPolYY_toggled(bool checked);
    void on_leTxAtt_valueChanged(int arg1);

    void on_BitBtn1_clicked();

    void on_rlsIP_textEdited(const QString &arg1);

    void on_rlsIP_textChanged(const QString &arg1);

    void on_rbRxAnt0_toggled(bool checked);

    void on_rbRxAnt1_toggled(bool checked);

    void on_cbMGEN_toggled(bool checked);

    void on_seMLEN_valueChanged(int arg1);

    void on_rlsPort_valueChanged(int arg1);

    void on_leRxAtt_valueChanged(int arg1);

    void on_rbRxPolXX_toggled(bool checked);

    void on_rbRxPolXY_toggled(bool checked);

    void on_rbRxPolYX_toggled(bool checked);

    void on_rbRxPolYY_toggled(bool checked);

private:
    Ui::formSettings *ui;
    int CurrentTxPacketNum;
};

#endif // FORMSETTINGS_H
