#include "inserttabledialog.h"
#include "ui_inserttabledialog.h"

InsertTableDialog::InsertTableDialog(RichTextProcess* editor1, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InsertTableDialog),
    editor(editor1)

{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose, true);
    this->setWindowTitle("插入表格");
    ui->width->setValue(200);
    ui->width->setMaximum(1000);
}

InsertTableDialog::~InsertTableDialog()
{
    delete ui;
}

void InsertTableDialog::on_pushButton_2_clicked()
{
    this->close();
}

void InsertTableDialog::on_pushButton_clicked()
{
    rowNum = ui->row->value();
    colNum = ui->col->value();
    cellWidth = ui->width->value();

    editor->insertTable(rowNum, colNum, cellWidth);
    this->close();
}
