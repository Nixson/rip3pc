#ifndef GWINDOW_H
#define GWINDOW_H

#include <QMainWindow>
#include <QPluginLoader>
#include <QProgressBar>
#include <QLabel>
#include "pluginterface.h"
#include "typelist.h"
#include "mcontrol.h"
#include "nomain/formsettings.h"
#include "nomain/debugdialog.h"
#include "nomain/rdata.h"
#include "nomain/savefile.h"
#include "nomain/controlform.h"
#include "memory.h"


namespace Ui {
class GWindow;
}

class GWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GWindow(QWidget *parent = 0);
    ~GWindow();
    DebugDialog* debug;

private slots:
    void loadPlugin(Plugs &plugin);
    void hidePlugin(Plugs &plugin);
    void on_mmTile_triggered();
    void on_mm3Dmodel_triggered();
    void on_mmGpolarization_triggered(bool checked);
    void on_mmVpolarization_triggered(bool checked);
    void on_mmDebug_triggered();
    void on_mmPlugs_triggered();
    void on_mmSignal_triggered();
    void on_mmLoad_triggered();
    void addDock(QString name, QString title);
    void log(QString);
    void sync();
    void progress(int);
    void progressTimerNum();
    void on_ArgMax_valueChanged(int value);

    void on_ArgMin_valueChanged(int value);

    void on_PhMin_valueChanged(int value);

    void on_Barier_valueChanged(int value);

    void on_mmRastr_triggered();

    void on_mmAFRastr_triggered();

    void on_mmSave_triggered();

    void on_mmControlForm_triggered();

    void on_mmOsc_triggered();

signals:
    void setMemory(Rdata *mLink);
    void init();
    void sendMsg(unsigned short BufferSize, unsigned char *Buffer, unsigned short CmdNum);
private:
    Ui::GWindow *ui;
    MControl *control;
    formSettings* fs;
    QMap<QString,QDockWidget *> dw;
    QProgressBar *pb;
    QLabel *logLabel, *infoLabel;
    QMap <QString, Plugs > plugins;
    QMap <QString, QAction *> plugActions;
    QMap <QString, PlugWidgetInterfaces *> plugObject;
    SaveFile *sf;
    ControlForm *cf;
};

#endif // GWINDOW_H
