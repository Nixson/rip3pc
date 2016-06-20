#ifndef DEBUGDIALOG_H
#define DEBUGDIALOG_H

#include <QWidget>

namespace Ui {
class DebugDialog;
}

class DebugDialog : public QWidget
{
    Q_OBJECT

public:
    explicit DebugDialog(QWidget *parent = 0);
    ~DebugDialog();
    void log(QString msg);

private:
    Ui::DebugDialog *ui;
};

#endif // DEBUGDIALOG_H
