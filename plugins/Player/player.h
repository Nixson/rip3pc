#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include "../../app/pluginterface.h"
#include "../../app/typelist.h"
#include "../../app/nomain/rdata.h"
#include <QTimer>


namespace Ui {
class Player;
}

class Player : public PlugWidgetInterfaces
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "ru.nixson.physicks.Rip3p.PlugWidgetInterfaces" FILE "Player.json")
    Q_INTERFACES(PlugWidgetInterfaces)

public:
    explicit Player();
    ~Player();
    void setMemory(Rdata *mLink) Q_DECL_OVERRIDE;
    void init() Q_DECL_OVERRIDE;

signals:
    void processStep(int);
    void processStop(int);
    void processStart();
    void sync();

public slots:
    void syncSlot();
    void nextStep();

private slots:
    void on_speed_valueChanged(int arg1);
    void on_screen_valueChanged(int arg1);
    void on_pause_clicked();
    void on_start_clicked();

private:
    Ui::Player *ui;
    int speed;
    int Size;
    int width;
    int Position;
    QTimer *mainTimer;
    Clowd GA, GP, VA, VP;
    Clowd vGA, vGP, vVA, vVP;
    Rdata *mem;
};

#endif // PLAYER_H
