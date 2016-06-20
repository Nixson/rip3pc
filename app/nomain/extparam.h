#ifndef EXTPARAM_H
#define EXTPARAM_H

#include <QWidget>

namespace Ui {
class ExtParam;
}

class ExtParam : public QWidget
{
    Q_OBJECT

public:
    explicit ExtParam(QWidget *parent = 0);
    ~ExtParam();
    void setData(QString name, QByteArray &dataArray);
signals:
    void load(QString name, QByteArray &data);
    void sync();
    void log(QString msg);

private slots:
    void on_upload_clicked();


    void on_sArg_valueChanged(int arg1);

    void on_sAng_valueChanged(int arg1);

private:
    Ui::ExtParam *ui;
    QString fname;
    QByteArray tmpData;
    int valArg, valAng, normal;
};

#endif // EXTPARAM_H
