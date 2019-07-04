#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QFontComboBox>
#include <QComboBox>
#include <QVBoxLayout>
#include <QFontDatabase>
#include <QTextCharFormat>
#include <QDialog>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QColor>
#include <QColorDialog>
#include <QPixmap>
#include "richtextprocess.h"
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void init();
    void setPaperMargin();
protected :
    void closeEvent(QCloseEvent*);
private slots:
    void textFamily(const QString&);
    void textSize(const QString&);
    void receiveLeftMargin(double);
    void receiveRightMargin(double);
    void receiveTopMargin(double);
    void receiveBottomMargin(double);
    void receiveSetMargainYesButton();
    void receiveSetMargainResetButton();
    void on_action_saveAs_triggered();
    void on_action_left_triggered();
    void on_action_right_triggered();
    void on_action_center_triggered();
    void on_action_bold_triggered();
    void on_action_italic_triggered();
    void on_action_underline_triggered();
    void on_action_newPage_triggered();
    void on_action_pageMargin_triggered();
    void on_action_deletePage_triggered();
    void on_action_insertImage_triggered();
    void on_action_color_triggered();
    void on_action_lineDistance_triggered();
    void on_action_undo_triggered();
    void on_action_redo_triggered();
    void on_action_table_triggered();
    void on_action_saveAsEdit_triggered();
    void on_action_open_ofdx_triggered();
    void on_action_new_triggered();
    void on_action_enlarge_triggered();
    void on_action_shrink_triggered(); 
    void changeStatusBar(int);

    void on_action_formatPainter_triggered();

    void on_action_pageDirection_triggered();

    /*
protected:
    void wheelEvent(QWheelEvent *event) ;
    bool eventFilter(QObject *obj, QEvent *event) ;
    */

    void on_action_count_triggered();
    void on_action_preview_triggered();

private:
    Ui::MainWindow *ui;

    QComboBox *fontSizeCombo;
    QFontComboBox *fontCombo;
    QScrollArea *scrollArea;
    RichTextProcess *editor;
    QDialog *pageMarginDialog;
    QDialog *lineDistanceDialog;
    QDialog *insertImageDialog;
    QDialog *countWordsDialog;
    QLabel* pageNum;
};

#endif // MAINWINDOW_H
