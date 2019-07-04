#ifndef INSERTIMAGEDIALOG_H
#define INSERTIMAGEDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include "richtextprocess.h"

namespace Ui {
class InsertImageDialog;
}

class InsertImageDialog : public QDialog
{
    Q_OBJECT

public:
    //默认参数值必须由右向左填写
    explicit InsertImageDialog(RichTextProcess* editor1, QWidget *parent = 0);
    ~InsertImageDialog();

private slots:
    void on_selectImage_clicked();
    void on_no_clicked();
    void on_yes_clicked();

    void receiveHeight(double);
    void receiveWidth(double);
    void on_resetScale_clicked();

private:
    Ui::InsertImageDialog *ui;
    RichTextProcess *editor;
    QString path;
    double height;
    double width;
};

#endif // INSERTIMAGEDIALOG_H
