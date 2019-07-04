#ifndef INSERTTABLEDIALOG_H
#define INSERTTABLEDIALOG_H

#include <QDialog>
#include "richtextprocess.h"

namespace Ui {
class InsertTableDialog;
}

class InsertTableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InsertTableDialog(RichTextProcess* editor1, QWidget *parent = 0);
    ~InsertTableDialog();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::InsertTableDialog *ui;
    RichTextProcess *editor;
    double cellWidth;
    int rowNum;
    int colNum;
};

#endif // INSERTTABLEDIALOG_H
