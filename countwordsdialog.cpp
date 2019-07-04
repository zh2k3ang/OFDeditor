#include "countwordsdialog.h"
#include "ui_countwordsdialog.h"

CountWordsDialog::CountWordsDialog(int words, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CountWordsDialog)
{
    ui->setupUi(this);
    this->words = words;
    ui->lbl_words->setText(QString::number(words));
}

CountWordsDialog::~CountWordsDialog()
{
    delete ui;
}
