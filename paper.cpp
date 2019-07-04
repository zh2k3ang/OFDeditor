#include "paper.h"

Paper::Paper(QWidget *parent):QTextEdit(parent)
{
    initPaper();
}

void Paper::initPaper()
{
    this->activateWindow();
    this->setFocus();
//    repaint();
    height = A4height;
    width = A4width;
    paperMargin = {30.0,30.0,30.0,30.0};
    lineDistance = 3.5;
    letterDistanceScale = 1.0;

    //设置字距
    QTextCharFormat charFmt;
    charFmt.setFontFamily("宋体");
    charFmt.setFontPointSize(11);
    charFmt.setFontLetterSpacing(letterDistanceScale*100);
    this->setCurrentCharFormat(charFmt);

    //设置基本字体
    QFont font;
    font.setPointSize(11);
    this->setFont(font);

    doc = this->document();
    rootFrame = doc->rootFrame();
    QTextFrameFormat frameFormat;
    frameFormat.setLeftMargin(paperMargin.leftMargin);
    frameFormat.setRightMargin(paperMargin.rightMargin);
    frameFormat.setTopMargin(paperMargin.topMargin);
    frameFormat.setBottomMargin(paperMargin.bottomMargin);
    rootFrame->setFrameFormat(frameFormat);

    connect(this, SIGNAL(textChanged()), this, SLOT(ifIsEmpty()));
    connect(this, SIGNAL(textEditIsEmpty()), this, SLOT(resetPaper()));   //防止内容为空后textEdit设置回到默认值。且作为一个判断函数
}

void Paper::setLeftMargin(double num)
{
    paperMargin.leftMargin = num;
    setFrameMargin();
}

void Paper::setRightMargin(double num)
{
    paperMargin.rightMargin = num;
    setFrameMargin();
}

void Paper::setTopMargin(double num)
{
    paperMargin.topMargin = num;
    setFrameMargin();
}

void Paper::setBottomMargin(double num)
{
    paperMargin.bottomMargin = num;
    setFrameMargin();
}

void Paper::setPaperMargin(double l, double r, double t, double b)
{
    paperMargin.leftMargin = l;
    paperMargin.rightMargin = r;
    paperMargin.topMargin = t;
    paperMargin.bottomMargin = b;
}

void Paper::setLineDistance(double l)
{
    QTextBlockFormat fmt;
    fmt.setLineHeight(l, QTextBlockFormat::LineDistanceHeight);
    this->selectAll();
    QTextCursor cur = this->textCursor();
    cur.setBlockFormat(fmt);
    lineDistance = l;
}

void Paper::setLetterDistanceScale(double l)
{
    letterDistanceScale = l;
    this->selectAll();
    QTextCharFormat charFmt;
    charFmt.setFontLetterSpacing(letterDistanceScale*100);
    this->mergeCurrentCharFormat(charFmt);
}

double Paper::getLineDistance()
{
    return lineDistance;
}

PaperMargin Paper::getPaperMargin()
{
    return paperMargin;
}

void Paper::setFrameMargin()
{
    QTextFrameFormat frameFormat;
    frameFormat.setLeftMargin(paperMargin.leftMargin);                 //必须把每个方向的边距都重新设置呀！不然frameFormat不知道！！！
    frameFormat.setRightMargin(paperMargin.rightMargin);
    frameFormat.setTopMargin(paperMargin.topMargin);
    frameFormat.setBottomMargin(paperMargin.bottomMargin);
    rootFrame = doc->rootFrame();               //解决打开的页面不能进行页面设置的bug
    rootFrame->setFrameFormat(frameFormat);
}

//添加图片
void Paper::insertImage(QString imagePath, double height, double width)
{
    QImage image = QImageReader(imagePath).read();
    QUrl Uri(QString ( "file://%1" ).arg (imagePath));
    doc->addResource( QTextDocument::ImageResource, Uri, QVariant(image));
    QTextCursor cursor = this->textCursor();
    QTextImageFormat imageFormat;
    imageFormat.setWidth(width);
    imageFormat.setHeight(height);
    imageFormat.setName(Uri.toString());
    cursor.insertImage(imageFormat);
}

void Paper::resetPaper()
{
    this->setFontFamily("宋体");
    this->setFontPointSize(11);
}

void Paper::ifIsEmpty()
{
    if(this->toPlainText().isEmpty())
       emit textEditIsEmpty();
}

void Paper::setHeight(double h)
{
    height = h;
}

void Paper::setWidth(double w)
{
    width = w;
}

double Paper::getHeight()
{
    return height;
}

double Paper::getWidth()
{
    return width;
}

QTextDocument* Paper::getPaperDocument()
{
    return doc;
}

int Paper::getPaperNum()
{
    return num;
}

void Paper::setPaperNum(int i)
{
    num = i;
}

double Paper::getLetterDistanceScale()
{
    return letterDistanceScale;
}

