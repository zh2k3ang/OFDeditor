#include "setlinedistancedialog.h"
#include "ui_setlinedistancedialog.h"

setLineDistanceDialog::setLineDistanceDialog(RichTextProcess* editor1, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setLineDistanceDialog),
    editor(editor1)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    ui->box->setValue(editor->getCurrentPaper()->getLineDistance());
}

setLineDistanceDialog::~setLineDistanceDialog()
{
    delete ui;
}

void setLineDistanceDialog::on_pushButton_2_clicked()
{
    this->close();
}

void setLineDistanceDialog::on_pushButton_clicked()
{
    this->distance = ui->box->value();
    editor->setlineDistance(distance, false);
    this->close();
}
