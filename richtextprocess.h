#ifndef RICHTEXTPROCESS_H
#define RICHTEXTPROCESS_H

#include <QWidget>
#include <QVBoxLayout>
#include <QVector>
#include <QTextTable>
#include <QTextDocumentWriter>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextBlockFormat>
#include <QFileDialog>
#include <QTextLength>
#include <QTextCodec>
#include <QMessageBox>
#include <QtAlgorithms>
#include <QtGui\5.8.0\QtGui\private\qzipwriter_p.h>
#include <QtGui\5.8.0\QtGui\private\qzipreader_p.h>
#include "informationextract.h"
#include "paper.h"
#include <QtPrintSupport/qprinter.h>
#include <QtPrintSupport/QPrintPreviewDialog>

const double defaultMargin = 10.0;
const double defaultSpace = 10.0;

class RichTextProcess:public QWidget
{
    Q_OBJECT
public:
    explicit RichTextProcess(QWidget *parent = 0);

    int getPaperNum();
    Paper* getCurrentPaper();

    void setPaperHeight(double);
    void setPaperWidth(double);
    double getPaperHeight();
    double getPaperWidth();

    void setFormat(const QTextCharFormat&);
    void saveAs();
    void saveFile(QString);
    void setAlign(int);
    void createNewPaper();
    void deleteLastPaper();
    bool saveToRichText();
    double calculateTotalHeight();
    void resizePage();
    void setlineDistance(double, bool);
    void setLetterDistanceScale(double, bool);
    bool openRichText();
    void resetEditor();
    bool editorIsEmpty();
    static QString extractRichText(int);
    static QMessageBox* showMessage();
    static bool cmp(Paper*, Paper*);
    void setPaperMarginToAll(double, double, double, double);
    void formatPainter(bool);
    void enlarge();
    void shrink();
    double getScale();
    void changePaperDirection();
    void insertImage(QString, double, double);
    void insertTable(double, double, double);
    int countWords();
    void preview();

private:
    int paperNum;
    double height;
    double width;
    QVBoxLayout *mainLayout;
    QVector<Paper*> paperContain;
    InformationExtract *informationExtract;
    int count;                            //用来确定多线程加载页面时是否全部加载完毕
    QVector<QString> htmlTmp;             //临时存放解析的html页面，解析完后排序输入
    QTextCharFormat painterSelected;      //用于格式刷存取文字格式
    double scale;                         //用于编辑器显示时进行缩放，初始化为1
    QVector<double> letterDistanceScaleContain;    //用于存储每一页的字距
public slots:
    void receiveLoadRichText(QString, Paper*);
    void printPreview(QPrinter*);
signals:
    void paperNumChanged(int);
};

#endif // RICHTEXTPROCESS_H
