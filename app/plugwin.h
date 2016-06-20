#ifndef PLUGWIN_H
#define PLUGWIN_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QPluginLoader>
#include "typelist.h"

namespace Ui {
class PlugWin;
}

class PlugWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlugWin(QWidget *parent = 0);
    ~PlugWin();
    void tableAppend(Plugs &p);
    void load();
signals:
    void sync();
    void loadPlugin(Plugs &plugin);
    void hidePlugin(Plugs &plugin);

private slots:
    void on_addPlug_clicked();
    void checkedItem(QTableWidgetItem *item, bool check);
    void checked(bool checked);

private:
    Ui::PlugWin *ui;
    QMap<QString, Plugs> plugList;
};

#endif // PLUGWIN_H
