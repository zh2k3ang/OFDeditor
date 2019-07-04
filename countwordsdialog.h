#ifndef COUNTWORDSDIALOG_H
#define COUNTWORDSDIALOG_H

#include <QDialog>

namespace Ui {
class CountWordsDialog;
}

class CountWordsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CountWordsDialog(int words = 0, QWidget *parent = 0);
    ~CountWordsDialog();

private:
    Ui::CountWordsDialog *ui;
    int words;
};

#endif // COUNTWORDSDIALOG_H
