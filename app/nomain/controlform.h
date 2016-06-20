#ifndef CONTROLFORM_H
#define CONTROLFORM_H

#include <QWidget>

namespace Ui {
class ControlForm;
}

class ControlForm : public QWidget
{
    Q_OBJECT

public:
    explicit ControlForm(QWidget *parent = 0);
    ~ControlForm();
signals:
    void sync();
    void sendMsg(unsigned short BufferSize, unsigned char *Buffer, unsigned short CmdNum);
private slots:
    void on_leNumberOfMultOsc_valueChanged(int arg1);
    void on_leMulOscDelay_valueChanged(int arg1);
    void on_leRasterPeriod_valueChanged(int arg1);
    void on_leScanSector_valueChanged(double arg1);

    void on_bContView32Avg_clicked();

private:
    Ui::ControlForm *ui;
};

#endif // CONTROLFORM_H
