#ifndef MCONTROL_H
#define MCONTROL_H

#include "typelist.h"
#include <QObject>
#include <QSettings>
#include <QTimer>
#include <QThread>
#include <QMdiArea>
#include <QMdiSubWindow>

#include "nomain/debugdialog.h"
#include "nomain/scobject.h"
#include "gr/gr3d.h"
#include "gr/plotpolarization.h"
#include "gr/plotraster.h"
#include "gr/plotosc.h"

#include "plugwin.h"
#include "worker.h"

class MControl : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    explicit MControl(QObject *parent = 0);
    ~MControl();
    void setAction(QString name, QVariant value);
    void winOpen(QString winName);
    void showGr3D(QString sType);
    void showPlotPolarization(QString sType);
    void showPlotRastr(QString sType);
    void showPlotOsc(QString sType);
    void setDock(QMap<QString,QDockWidget *> &dock);
    void init();
    void load();
    void setDev(QString);
    DebugDialog* debug;
    PlugWin* plugin;
    QMdiSubWindow *gr3dVmdi, *gr3dGmdi, *grPlotVmdi, *grPlotGmdi, *grRPlotVmdi, *grRPlotGmdi, *grOPlotVmdi, *grOPlotGmdi;
    bool isgr3dVmdi, isgr3dGmdi, isgrPlotVmdi, isgrPlotGmdi, isgrRPlotVmdi, isgrRPlotGmdi, isgrOPlotVmdi, isgrOPlotGmdi;
    Gr3D *gr3dV, *gr3dG;
    PlotPolarization *grPlotV, *grPlotG;
    PlotRaster *grRPlotV, *grRPlotG;
    PlotOsc *grOPlotV, *grOPlotG;
    void setMDI(QMdiArea *lnk);
    QMdiArea *area;
    ScObject *scV, *scG;

signals:
    void loadSrc(QByteArray &data);
    void loadFinished(QByteArray &data);
    void loadFinishedF(QByteArray &data);
    void setProgress(int);
    void setLog(QString);
    void sync();
    void loadPlugin(Plugs &plugin);
    void hidePlugin(Plugs &plugin);
    void sendParamSignals();
    void sendMsgSignal(unsigned short BufferSize, unsigned char *Buffer, unsigned short CmdNum);

public slots:
    void isgr3dVmdiHide();
    void isgr3dGmdiHide();
    void isgrPlotVmdiHide();
    void isgrPlotGmdiHide();
    void isgrRPlotVmdiHide();
    void isgrRPlotGmdiHide();
    void isgrOPlotVmdiHide();
    void isgrOPlotGmdiHide();
    void saveConfig();
    void saveConfigTimer();
    void log(QString);
    void progress(int);
    void shared(int);
    void resultXX(Clowd &dataA, Clowd &dataH);
    void resultYY(Clowd &dataA, Clowd &dataH);
    void loadData(QString fileName, QByteArray &data);
    void sendParam();
    void sendMsg(unsigned short BufferSize, unsigned char *Buffer, unsigned short CmdNum);

private:
    QSettings* settings;
    Worker *worker;

    bool hasGData, hasVData;
    QTimer *saveTimer;
};

#endif // MCONTROL_H
