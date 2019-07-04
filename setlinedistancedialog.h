#ifndef SETLINEDISTANCEDIALOG_H
#define SETLINEDISTANCEDIALOG_H

#include <QDialog>
#include "richtextprocess.h"

namespace Ui {
class setLineDistanceDialog;
}

class setLineDistanceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit setLineDistanceDialog(RichTextProcess* editor1, QWidget *parent = 0);
    ~setLineDistanceDialog();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::setLineDistanceDialog *ui;
    RichTextProcess *editor;
    double distance;
};

#endif // SETLINEDISTANCEDIALOG_H
