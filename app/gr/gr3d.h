#ifndef GR3D_H
#define GR3D_H

#include <QMainWindow>
#include "../nomain/scobject.h"
#include "plotgl.h"

namespace Ui {
class Gr3D;
}

class Gr3D : public QMainWindow
{
    Q_OBJECT

public:
    explicit Gr3D(QWidget *parent = 0);
    ~Gr3D();
    PlotGl *pgl;
    void setType(QString typeName);
    void updates();
signals:
    void sync();
private slots:
    void on_SlideScale_valueChanged(int value);

    void on_SlideZ_valueChanged(int value);

    void on_SlideX_valueChanged(int value);

    void on_SlideY_valueChanged(int value);

    void xRotationChanged(int);
    void yRotationChanged(int);
    void zRotationChanged(int);

private:
    Ui::Gr3D *ui;
    QString tName;
};

#endif // GR3D_H
