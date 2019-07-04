#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "insertimagedialog.h"
#include "pagesetupdialog.h"
#include "inserttabledialog.h"
#include "setlinedistancedialog.h"
#include "countwordsdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{

    //初始化主窗体
    setWindowState(Qt::WindowMaximized);
    setWindowTitle("Editor文本编辑系统");

    //添加窗口中心滚动区
    scrollArea = new QScrollArea(this);                      //MainWindow中心部件为滚动区
    scrollArea->setBackgroundRole(QPalette::Dark);           //滚动区灰色
    setCentralWidget(scrollArea);

    //在中心滚动区添加页面
    editor = new RichTextProcess(this);
    editor->resize(editor->getPaperWidth()*editor->getScale(),editor->getPaperHeight()*editor->getPaperNum()*editor->getScale());         //重新设置editor窗口大小，A4比例大小
    scrollArea->setWidget(editor);                           //滚动区内添加文本编辑区
    scrollArea->setAlignment(Qt::AlignCenter);               //是文本编辑框居中对齐

    //添加字体选择 OFD阅读器不支持Qt中支持的很多字体！！！！！！！！！
    fontCombo = new QFontComboBox(this);
    ui->toolBar->addWidget(fontCombo);                       //designer界面字体选择框添加不到工具栏中？用代码实现
    fontCombo->setStatusTip("更改字体");
    connect(fontCombo, SIGNAL(activated(QString)),this,SLOT(textFamily(QString)));      //连接字体选择框的槽

    //添加字号选择
    fontSizeCombo = new QComboBox(this);
    ui->toolBar->addWidget(fontSizeCombo);
    fontSizeCombo->setEditable(true);
    fontSizeCombo->setStatusTip("更改字号");
    fontSizeCombo->addItem("11");                            //与编辑框初始编辑字号相对应
    QFontDatabase db;                                        //字体库中选择
    foreach(int size, db.standardSizes())
        fontSizeCombo->addItem(QString::number(size));
    connect(fontSizeCombo, SIGNAL(activated(QString)), this, SLOT(textSize(QString)));

    //添加状态栏
    pageNum = new QLabel();
    pageNum->setText("共1页");
    statusBar()->addWidget(pageNum);
    connect(editor, SIGNAL(paperNumChanged(int)), this, SLOT(changeStatusBar(int)));
}

//创建字体选择框，改变字体时所需要的函数
void MainWindow::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    editor->setFormat(fmt);
}

//创建字号选择框，改变字号时所需要的函数
void MainWindow::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if(p.toFloat()>0)
    {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        editor->setFormat(fmt);
    }
}

void MainWindow::on_action_saveAs_triggered()
{
    editor->saveAs();
}

void MainWindow::on_action_left_triggered()
{
    editor->setAlign(0);
}

void MainWindow::on_action_right_triggered()
{
    editor->setAlign(1);
}

void MainWindow::on_action_center_triggered()
{
    editor->setAlign(2);
}

void MainWindow::on_action_bold_triggered()
{

    QTextCharFormat fmt;
    QTextCursor cursor = editor->getCurrentPaper()->textCursor();
    fmt = cursor.charFormat();
    if(fmt.fontWeight()==QFont::Bold)
        fmt.setFontWeight(QFont::Normal);
    else
        fmt.setFontWeight(QFont::Bold);
    ui->action_bold->setChecked(false);
    editor->setFormat(fmt);
}

void MainWindow::on_action_italic_triggered()
{
    QTextCharFormat fmt;
    QTextCursor cursor = editor->getCurrentPaper()->textCursor();
    fmt = cursor.charFormat();
    fmt.setFontItalic(!fmt.fontItalic());
    ui->action_italic->setChecked(false);
    editor->setFormat(fmt);
}

void MainWindow::on_action_underline_triggered()
{
    QTextCharFormat fmt;
    QTextCursor cursor = editor->getCurrentPaper()->textCursor();
    fmt = cursor.charFormat();
    fmt.setFontUnderline(!fmt.fontUnderline());
    ui->action_underline->setChecked(false);
    editor->setFormat(fmt);
}

void MainWindow::on_action_formatPainter_triggered()
{
   editor->formatPainter(ui->action_formatPainter->isChecked());
}

void MainWindow::on_action_deletePage_triggered()
{
    editor->deleteLastPaper();                //此处调用之前自动删除页面所用的槽函数，可行。
    editor->resizePage();   //此处页面大小需要计算
}

void MainWindow::on_action_newPage_triggered()
{
    editor->createNewPaper();
    editor->resizePage();   //此处页面大小需要计算
}

void MainWindow::on_action_insertImage_triggered()
{
    InsertImageDialog *dialog = new InsertImageDialog(editor, this);
    dialog->exec();

    //交由富文本处理类实现添加图片功能
  //  editor->insertImage();
}

void MainWindow::receiveLeftMargin(double num)
{
     editor->getCurrentPaper()->setLeftMargin(num);
}

void MainWindow::receiveRightMargin(double num)
{
     editor->getCurrentPaper()->setRightMargin(num);
}

void MainWindow::receiveTopMargin(double num)
{
     editor->getCurrentPaper()->setTopMargin(num);
}

void MainWindow::receiveBottomMargin(double num)
{
     editor->getCurrentPaper()->setBottomMargin(num);
}

void MainWindow::receiveSetMargainYesButton()
{
    pageMarginDialog->close();
}

void MainWindow::receiveSetMargainResetButton()
{
    receiveLeftMargin(30.00);
    receiveRightMargin(30.00);
    receiveTopMargin(30.00);
    receiveBottomMargin(30.00);
    pageMarginDialog->close();
}

void MainWindow::on_action_pageMargin_triggered()
{
    PageSetupDialog *dialog = new PageSetupDialog(editor, this);
    dialog->exec();
}

void MainWindow::on_action_color_triggered()
{
    QColor color = QColorDialog::getColor(Qt::black);
    if(!color.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(color);
    editor->setFormat(fmt);
}

void MainWindow::on_action_lineDistance_triggered()
{
    setLineDistanceDialog *dialog = new setLineDistanceDialog(editor, this);
    dialog->exec();
}

void MainWindow::on_action_undo_triggered()
{
    this->editor->getCurrentPaper()->undo();
}

void MainWindow::on_action_redo_triggered()
{
    this->editor->getCurrentPaper()->redo();
}

void MainWindow::on_action_table_triggered()
{
    InsertTableDialog *dialog = new InsertTableDialog(editor, this);
    dialog->exec();
}

void MainWindow::on_action_saveAsEdit_triggered()
{
    editor->saveToRichText();
}

void MainWindow::on_action_open_ofdx_triggered()
{
    editor->openRichText();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //判断文本编辑区是否有内容，若有则先提示保存，再重置界面
    if(!editor->editorIsEmpty())
    {
        QMessageBox* msgBox = RichTextProcess::showMessage();
        int ret = msgBox->exec();
        if (ret == QMessageBox::Save)
        {
            editor->saveToRichText();
            editor->resetEditor();
        }
        else if (ret == QMessageBox::Discard)
            editor->resetEditor();
        else
            event->ignore();
        msgBox->deleteLater();
    }
}

void MainWindow::on_action_new_triggered()
{
    if(!editor->editorIsEmpty())
    {
        QMessageBox* msgBox = RichTextProcess::showMessage();
        int ret = msgBox->exec();
        if (ret == QMessageBox::Save)
        {
            editor->saveToRichText();
            editor->resetEditor();
        }
        else if (ret == QMessageBox::Discard)
            editor->resetEditor();
        msgBox->deleteLater();
    }
}

void MainWindow::changeStatusBar(int i)
{
    pageNum->setText("共"+QString::number(i)+"页");
}

void MainWindow::on_action_pageDirection_triggered()
{
    editor->changePaperDirection();
}



void MainWindow::on_action_enlarge_triggered()
{
    editor->enlarge();
}

void MainWindow::on_action_shrink_triggered()
{
  //  editor->shrink();
//    QTextCharFormat charFmt;
//    editor->getCurrentPaper()->selectAll();
  //  editor->getCurrentPaper()->mergeCurrentCharFormat(charFmt);
  //  editor->getCurrentPaper()->mergeCurrentCharFormat(charFmt);

    editor->getCurrentPaper()->zoomIn();
    qDebug() << "zoomIn";
}


/*
// 滚轮事件
void MainWindow::wheelEvent(QWheelEvent *event)
{

    // 当滚轮远离使用者时进行放大，当滚轮向使用者方向旋转时进行缩小
    if(event->delta() > 0){
        editor->getCurrentPaper()->zoomIn();
    }else{
        qDebug() << "zoomout";
        editor->getCurrentPaper()->zoomOut();
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
//  // 事件过滤器
//  // 如果是textEdit部件上的事件
  if(obj == editor->getCurrentPaper()){
    if(event->type() == QEvent::KeyPress) {
      QKeyEvent *kevent = dynamic_cast<QKeyEvent *>(event);
      if (kevent->key() == Qt::Key_Down) {
          qDebug() << "zoomout";
        editor->getCurrentPaper()->zoomOut();

      }
      else if(kevent->key() == Qt::Key_Up) {
        editor->getCurrentPaper()->zoomIn();
      }
    }
  }
  return QMainWindow::eventFilter(obj,event);
}
*/



void MainWindow::on_action_count_triggered()
{
    CountWordsDialog *dialog = new CountWordsDialog(editor->countWords());
    dialog->exec();
}


void MainWindow::on_action_preview_triggered()
{
    editor->preview();
}
