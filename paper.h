#ifndef PAPER_H
#define PAPER_H
#include <QTextEdit>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextFrameFormat>
#include <QDebug>
#include <QImage>
#include <QImageReader>
#include <QMimeData>
#include <QFileInfo>


//ofd文件按照正常值*5的比例进行变化存取
const double A4width =  210*5;
const double A4height = 297*5;
const double B5width = 210*5;
const double B5height = 276*5;
const double A5width = 148*5;
const double A5height = 210*5;

typedef struct paperMargin
{
    double topMargin;
    double bottomMargin;
    double leftMargin;
    double rightMargin;
}PaperMargin;

class Paper : public QTextEdit
{
    Q_OBJECT
public:
    Paper(QWidget *parent = 0);

    void initPaper();

    void setHeight(double);
    void setWidth(double);
    double getHeight();
    double getWidth();
    void setPaperMargin(double, double, double, double);
    void setLeftMargin(double);
    void setRightMargin(double);
    void setTopMargin(double);
    void setBottomMargin(double);
    void setFrameMargin();
    void setLineDistance(double);
    void setLetterDistanceScale(double);
    double getLetterDistanceScale();
    double getLineDistance();
    PaperMargin getPaperMargin();
    QTextDocument* getPaperDocument();
    void insertImage(QString, double, double);
    int getPaperNum();
    void setPaperNum(int);
private:
    QTextDocument *doc;
    QTextFrame *rootFrame;
    PaperMargin paperMargin;

    double height;
    double width;
    int num;            //加载htmlye'm页面时计数用
    double lineDistance;
    double letterDistanceScale;    //字距比例
signals:
    void textEditIsEmpty();
public slots:
    void ifIsEmpty();
    void resetPaper();
};

#endif // PAPER_H
