#ifndef PAGESETUPDIALOG_H
#define PAGESETUPDIALOG_H

#include <QDialog>
#include "richtextprocess.h"

namespace Ui {
class PageSetupDialog;
}

class PageSetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PageSetupDialog(RichTextProcess* editor1, QWidget *parent = 0);
    ~PageSetupDialog();

private slots:
    void on_confirm_clicked();
    void on_cancel_clicked();
    void receiveLeftMargin(double);
    void receiveRightMargin(double);
    void receiveTopMargin(double);
    void receiveBottomMargin(double);
    void receiveLineHeight(double);
    void receiveLetterDistance(double);

    void on_pushButton_2_clicked();

private:
    Ui::PageSetupDialog *ui;
    RichTextProcess *editor;

    double leftMargin;
    double rightMargin;
    double topMargin;
    double bottomMargin;
    double lineHeight;
    double letterDistanceScale;
    double height;
    double width;
};

#endif // PAGESETUPDIALOG_H
