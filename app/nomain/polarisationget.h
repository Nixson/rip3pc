#ifndef POLARISATIONGET_H
#define POLARISATIONGET_H

#include <QWidget>

namespace Ui {
class PolarisationGet;
}

class PolarisationGet : public QWidget
{
    Q_OBJECT

public:
    explicit PolarisationGet(QWidget *parent = 0);
    ~PolarisationGet();
signals:
    void sync();
private slots:
    void on_aPhi1_valueChanged(int arg1);

    void on_aPhi2_valueChanged(int arg1);

    void on_sPhi1_valueChanged(int arg1);

    void on_sPhi2_valueChanged(int arg1);

    void on_aPhi1_2_valueChanged(int arg1);

    void on_aPhi2_2_valueChanged(int arg1);

    void on_sPhi1_2_valueChanged(int arg1);

    void on_sPhi2_2_valueChanged(int arg1);

private:
    Ui::PolarisationGet *ui;
};

#endif // POLARISATIONGET_H
