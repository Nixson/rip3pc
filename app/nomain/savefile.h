#ifndef SAVEFILE_H
#define SAVEFILE_H

#include <QWidget>

namespace Ui {
class SaveFile;
}

class SaveFile : public QWidget
{
    Q_OBJECT

public:
    explicit SaveFile(QWidget *parent = 0);
    ~SaveFile();

signals:
    void sync();

private slots:
    void on_checkBox_toggled(bool checked);
    void on_checkBox_2_toggled(bool checked);
    void on_editFolder_clicked();
    void on_saveButton_clicked();
    void on_experimentName_textChanged(const QString &arg1);
    void on_experimentName_textEdited(const QString &arg1);

private:
    Ui::SaveFile *ui;
};

#endif // SAVEFILE_H
