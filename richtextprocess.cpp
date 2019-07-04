#include "richtextprocess.h"
#include "thread_loadrichtext.h"

RichTextProcess::RichTextProcess(QWidget *parent) : QWidget(parent)
{
    paperNum = 1;
    //纸张实际大小
    height = A4height;
    width = A4width;
    scale = 1;

    mainLayout = new QVBoxLayout(this);
    Paper *paper = new Paper(this);
    mainLayout->addWidget(paper);
    mainLayout->setMargin(defaultMargin*scale);
    mainLayout->setSpacing(defaultSpace*scale);              //两张稿纸之间的留白，暂设置为0，在设置editor的大小时应该计算到
    paperContain.append(paper);                        //加入容器
}

int RichTextProcess::getPaperNum()
{
    return paperNum;
}

Paper* RichTextProcess::getCurrentPaper()
{
    Paper* paper = NULL;
    QWidget *current = this->focusWidget();
    for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
    {
        if((*it) == current)
            paper = (*it);
    }
    return paper;
}

//设置文字格式
void RichTextProcess::setFormat(const QTextCharFormat &format)
{
    QTextCursor cursor = this->getCurrentPaper()->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    this->getCurrentPaper()->mergeCurrentCharFormat(format);
}

//格式刷功能
void RichTextProcess::formatPainter(bool checked)
{
    QTextCursor textCursor = this->getCurrentPaper()->textCursor();
    if(checked)
        painterSelected = textCursor.charFormat();
    else
    {
        textCursor.mergeCharFormat(painterSelected);
        this->getCurrentPaper()->mergeCurrentCharFormat(painterSelected);
    }

    for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
    {
        if(checked)
            (*it)->viewport()->setCursor(QPixmap(":resources/format_mouse.png"));    //冒号指向资源目录
        else
            (*it)->viewport()->setCursor(Qt::IBeamCursor);
    }
}

//保存页面
void RichTextProcess::saveAs()
{
    QString filename = QFileDialog::getSaveFileName(this, "另存为", "新建OFD文档.ofd", "OFD 文档");
    if(filename.isEmpty())
        return;
    return saveFile(filename);
}

//用于保存richText，做打开使用
bool RichTextProcess:: saveToRichText()
{
    int i=0;
    //获得文件名
    QString filename = QFileDialog::getSaveFileName(this, tr("待编辑文件"), "新建OFDX文件.ofdx", "OFDX 文档");
    if (!(filename.endsWith(".ofdx", Qt::CaseInsensitive))) {
        filename += ".ofdx";
    }

    QZipWriter zip_writer(filename);

    for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
    {
        QFile file(QString("paper_%1.html").arg(QString::number(i)));
        if(!file.open(QFile::ReadWrite)) return false;
        QTextDocumentWriter writer;
        writer.setDevice(&file);
        writer.write((*it)->document());
        file.close();

        //添加到ofdx
        if(!file.open(QFile::ReadWrite)) return false;    //在这里file要重新打开，因为被writer占用所以会出现文件打开失败
        zip_writer.addFile(QString("paper_%1.html").arg(QString::number(i)), file.readAll());
        file.close();
        QFile::remove(QString("paper_%1.html").arg(QString::number(i)));
        i++;
    }
    //写入配置文件，包括页面方向，页面大小信息等
    QFile file("pageConfigure.txt");
    if(!file.open(QFile::ReadWrite)) return false;
    QTextStream text;
    text.setDevice(&file);
    text << "height:" << height << endl << "width:" << width << endl;
    qDebug() << "height:" << height << "width:" << width;
    text << "letterDistanceScale:" << endl;

    for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
    {
        text << (*it)->getLetterDistanceScale() << endl;
    }
    file.close();

    //添加到ofdx
    if(!file.open(QFile::ReadWrite)) return false;    //在这里file要重新打开，因为被writer占用所以会出现文件打开失败
    zip_writer.addFile("pageConfigure.txt", file.readAll());
    file.close();
    QFile::remove("pageConfigure.txt");

    zip_writer.close();
    return true;
}

bool RichTextProcess::editorIsEmpty()
{
    bool editorIsEmpty = true;
    //判断文本编辑区是否有内容，若有则先提示保存，再重置界面
    for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
    {
        if(!(*it)->document()->isEmpty())
        {
            editorIsEmpty = false;
            break;
        }
    }
    return editorIsEmpty;
}

//这是子线程运行的函数,解析页面
QString RichTextProcess::extractRichText(int i)
{
    qDebug() << "读取单页富文本文档" << i;
    QFile file;
    file.setFileName("./paper_"+QString::number(i)+".html");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << file.fileName();
    }
    //加载html输出到编辑区
    QByteArray data = file.readAll();
    QTextCodec *codec = Qt::codecForHtml(data);
    QString str = codec->toUnicode(data);
    file.close();
    return str;
}

bool RichTextProcess::cmp(Paper* a, Paper* b)
{
    return a->getPaperNum() - b->getPaperNum();
}

void RichTextProcess::receiveLoadRichText(QString str, Paper* paper)
{
  //  htmlTmp.append(str);
    paper->setHtml(str);
    //qDebug() << "加载了页面" << paper <<paper->getPaperNum();
    count++;
    if(count==paperContain.size())       //验证解析完毕
    {
        QDir dir;
        //删除临时文件夹ofdxing
        dir.setCurrent("../"); //文件目录设置到上一层
        OFDProcess::DeleteDirectory("ofdxing");

        //等页面内容加载完之后才能设置字距
        QVector<double>::iterator l = letterDistanceScaleContain.begin();
        for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
        {
            (*it)->setLetterDistanceScale(*l);
            qDebug() << "每页的字距"<<*l;
            l++;
        }
        letterDistanceScaleContain.clear();
    }
}

//打开ofdx文件
bool RichTextProcess::openRichText()
{
    htmlTmp.clear();           //先清空临时容器
    QString filename = QFileDialog::getOpenFileName(this, tr("选择OFDX文档"), ".", tr("OFDX (*.ofdx)"));
    if(filename==".ofdx"||filename.isEmpty())
       return false;

    //解压ofdx文件
    QDir dir;
    dir.mkdir("./ofdxing");
    QZipReader reader(filename);
    reader.extractAll("./ofdxing");

    //重置editor
    resetEditor();

    //遍历文件夹，取得所有文件
    dir.setCurrent("./ofdxing");
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Size | QDir::Reversed);
    QFileInfoList list = dir.entryInfoList();

    //首先为所有文件创建稿纸（因为窗口焦点没有控制好，这里使用两个循环有些繁琐）
    for(int i=0; i < list.size()-2; ++i)    //去掉配置文件和第一页
    {
        createNewPaper();
        resizePage();
    }

    //文件和页面容器一起遍历，文件加载到容器当中
    QVector<Paper*>::iterator it = paperContain.begin();
    count = 0;             //页面计数
    for (int i = 0; i < list.size()-1; ++i, ++it)     //去掉配置文件
    {

        //多线程加载页面
        Thread_loadRichText *thread = new Thread_loadRichText(i, list, (*it));
        connect(thread, SIGNAL(hasDone(QString, Paper*)), this, SLOT(receiveLoadRichText(QString, Paper*)));
        (*it)->setPaperNum(i);
        qDebug() << "页面号"<<(*it)->getPaperNum();
        thread->start();
    }

    //解析pageConfigure.txt
    //Qt得文件路径有点混乱
    QFile file;
    file.setFileName("../ofdxing/pageConfigure.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() <<"打开文件出错";
        return false;
    }
    QTextStream text;
    text.setDevice(&file);

    bool flag = false;
    while(!text.atEnd())
    {
        QString info =  text.readLine();
        qDebug() << info;
        //解析包含信息得字符串
        if(info.contains("height"))
            height = info.right(info.size()-7).toInt();
        if(info.contains("width"))
            width = info.right(info.size()-6).toInt();
        if(flag == true)
            letterDistanceScaleContain.append(info.toDouble());
        if(info=="letterDistanceScale:")
            flag = true;
    }

    setPaperHeight(height);
    setPaperWidth(width);
    qDebug() << "打开页面：height:" << height << "\n" << "width:" << width;
    resizePage();
    file.close();
    return true;
}

void RichTextProcess::resetEditor()
{
    //删除所有的页面，并添加一个页面
    for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
    {
        mainLayout->removeWidget((*it)); //卸载稿纸
        (*it)->deleteLater();               //清内存
        paperNum--;
    }
    paperContain.clear();                //清容器
    createNewPaper();                    //添加第一张稿纸
    resizePage();
}

void RichTextProcess::setAlign(int i)
{
    switch(i)
    {
    case 0:
        this->getCurrentPaper()->setAlignment(Qt::AlignLeft|Qt::AlignAbsolute);
        break;
    case 1:
        this->getCurrentPaper()->setAlignment(Qt::AlignRight|Qt::AlignAbsolute);
        break;
    case 2:
        this->getCurrentPaper()->setAlignment(Qt::AlignCenter);
        break;
    case 3:
        this->getCurrentPaper()->setAlignment(Qt::AlignJustify);
        break;
    }
}

//新建页面
void RichTextProcess::createNewPaper()
{
    Paper *paper = new Paper(this);
    paperContain.append(paper);                    //将页面加入到容器当中
    mainLayout->addWidget(paper);
    paperNum++;
    emit paperNumChanged(paperNum);
}

void RichTextProcess::deleteLastPaper()
{
    //需要判断是不是第一页
    Paper *lastPaper = paperContain.last();
    if(paperContain.first() == paperContain.last())
        return;
    mainLayout->removeWidget(lastPaper);  //卸载该控件

    //QT 5
    //paperContain.removeLast();          //容器中删除
    lastPaper->deleteLater();             //清内存
    paperContain.removeLast();            //删地址
    paperNum--;
    paperContain.last()->setFocus();
    emit paperNumChanged(paperNum);
}

void RichTextProcess::setPaperHeight(double h)
{
    height = h;
}

void RichTextProcess::setPaperWidth(double w)
{
    width = w;
}

double RichTextProcess::getPaperHeight()
{
    return height;
}

double RichTextProcess::getPaperWidth()
{
    return width;
}

double RichTextProcess::calculateTotalHeight()
{
    return height*paperNum+2*defaultMargin+(paperNum-1)*defaultSpace;
}

void RichTextProcess::resizePage()
{
    //重置了每个页面的高度和宽度
    //这里每一页都需要提取页面大小，但是所有页面大小都是集体控制
    for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
    {
        (*it)->setHeight(height);
        (*it)->setWidth(width);
    }
    mainLayout->setMargin(defaultMargin*scale);
    mainLayout->setSpacing(defaultSpace*scale);
    resize(width*scale,calculateTotalHeight()*scale);
}

void RichTextProcess::saveFile(QString filename)
{
    informationExtract = new InformationExtract;
    informationExtract->RichTextToOFD(paperContain, filename);        //完成转换及保存的一系列工作
}

void RichTextProcess::setlineDistance(double h, bool all)
{
    if(all)
        for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
            (*it)->setLineDistance(h);
    else
        this->getCurrentPaper()->setLineDistance(h);
}

void RichTextProcess::setLetterDistanceScale(double l, bool all)
{
    if(all)
        for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
            (*it)->setLetterDistanceScale(l);
    else
        this->getCurrentPaper()->setLetterDistanceScale(l);
}

//添加图片功能
void RichTextProcess::insertImage(QString imagePath, double height, double width)
{
    Paper* paper = getCurrentPaper();
    //交给Paper具体处理
    paper->insertImage(imagePath, height, width);
}

//添加表格功能
void RichTextProcess::insertTable(double row, double col, double width)
{
    Paper* paper = this->getCurrentPaper();
    QTextCursor cur(paper->textCursor());
    QTextLength len(QTextLength::FixedLength, width);
    QVector<QTextLength> constaint(col,len);

    QTextTableFormat tableFormat;
    tableFormat.setCellSpacing(1);
    tableFormat.setColumnWidthConstraints(constaint);
    QTextTable* table = cur.insertTable(row, col, tableFormat);


    //设置表格初始字体格式
    QTextCharFormat format;
    format.setFontFamily("宋体");
    format.setFontPointSize(11);
    for(int i=0; i<row; i++)
        for(int j=0; j<col; j++)
        {
            QTextTableCell cell = table->cellAt(i, j);
            cell.setFormat(format);
        }
}

QMessageBox* RichTextProcess::showMessage()
{
    QMessageBox* msgBox = new QMessageBox();
    msgBox->setText(tr("编辑区内有内容"));
    msgBox->setInformativeText(tr("是否保存未OFDX文档?"));
    msgBox->setStandardButtons( QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox->setButtonText (QMessageBox::Save,QString("保 存"));
    msgBox->setButtonText (QMessageBox::Discard,QString("不保存"));
    msgBox->setButtonText (QMessageBox::Cancel,QString("取 消"));
    return msgBox;
}

void RichTextProcess::setPaperMarginToAll(double leftMargin, double rightMargin, double topMargin,double bottomMargin)
{
    for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
    {
        (*it)->setLeftMargin(leftMargin);
        (*it)->setRightMargin(rightMargin);
        (*it)->setTopMargin(topMargin);
        (*it)->setBottomMargin(bottomMargin);
    }
}

void RichTextProcess::changePaperDirection()
{
    double tmp = getPaperHeight();
    setPaperHeight(getPaperWidth());
    setPaperWidth(tmp);
    resizePage();
}

double RichTextProcess::getScale()
{
    return scale;
}

void RichTextProcess::enlarge()
{
    scale = 1.5;
 //   resizePage();
    for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
    {
        (*it)->zoomIn(1.5);
        qDebug() << "我在变字体大小";
    }

}

void RichTextProcess::shrink()
{
    scale = 0.5;
    //遍历改变所有字体大小
    resizePage();
    for(QVector<Paper*>::iterator it = paperContain.begin();it!=paperContain.end();it++)
    {
        /*
        //改变字体大小
        QTextFrame *rootFrame = (*it)->document()->rootFrame();
        for(QTextFrame::iterator it = rootFrame->begin(); !it.atEnd(); it++)
        {
            QTextBlock block = it.currentBlock();
            for(QTextBlock::iterator it = block.begin(); !it.atEnd(); it++)
            {
                QTextFragment fragment = it.fragment();
                QTextCharFormat charFmt = fragment.charFormat();
                charFmt.setFontPointSize(charFmt.fontPointSize()*scale);
            }
        }*/
        ;

        (*it)->zoomIn(1);
        qDebug() << "改变";
    }
}

int RichTextProcess::countWords(){
    int length = 0;
    for(QVector<Paper*>::iterator it = paperContain.begin(); it!=paperContain.end(); it++)
    {
        Paper *paper = *it;
        QTextFrame *rootFrame = paper->document()->rootFrame();
        QTextBlock block;

        //遍历一个框架，根框架，迭代器取得其中的文本块
        for(QTextFrame::iterator it = rootFrame->begin(); !it.atEnd(); it++)
        {
            block = it.currentBlock();

                 //遍历文本块中的文本片段,文本输出主要用到的信息
                for(QTextBlock::iterator it = block.begin(); !it.atEnd(); it++)
                {
                    //判断是否是图片资源
                    QTextFragment fragment = it.fragment();

                    length += fragment.text().length();
                }
        }
    }
    qDebug() << "字数统计" << length;
    return length;
}

void RichTextProcess::preview(){
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreview(QPrinter*)));
    preview.exec();
}

void RichTextProcess::printPreview(QPrinter* printer){
    this->getCurrentPaper()->print(printer);
}



