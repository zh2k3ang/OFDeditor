#include "informationextract.h"
#include "thread_getpagetext.h"

int InformationExtract::imageNum = 0;
InformationExtract::InformationExtract(QObject* parent): QObject(parent)
{

}

void InformationExtract::RichTextToOFD(QVector<Paper*> paperContain, QString f)
{
    //PageInfo pageInfo;
    filename = f;
    pageNum = paperContain.size();

    //解析一个页面的信息
    int i = 0;
    for(QVector<Paper*>::iterator it = paperContain.begin(); it!=paperContain.end(); it++, i++)
    {
        //利用多线程提取页面内容
       Thread_GetPageText *thread = new Thread_GetPageText(*it, i);
       connect(thread, SIGNAL(hasDone(PageInfo)),
                        this, SLOT(receiveThread(PageInfo)));
       thread->start();
    }
//   writeToO(filename);                      //创建对象，并调用写入。
}

//对容器中的页面进行排序
bool InformationExtract::cmp(const PageInfo &a, const PageInfo &b)
{
    return a.number < b.number;
}

void InformationExtract::receiveThread(PageInfo pageInfo)
{
    qDebug() << "我收到了线程的信号";
    qDebug() << "页号接受"<< pageInfo.number;
    qDebug() << pageInfo.pageMargin.leftMargin;
    //取得单张页面信息
//    pageInfo.lineDistance = lineDistance;    //取得行距。败笔！！！待修改
    pageInfoContain.append(pageInfo);

    //若页面提取完毕，则交由ofdprocess处理
    if(pageInfoContain.size() == pageNum)
    {
        //多线程读取页面的速度不一，所以这里将页面按照页数重新排序
        std::sort(pageInfoContain.begin(), pageInfoContain.end(), cmp);
        writeToOFD(filename);                      //创建ofd对象，并调用写入。
    }

}

//文档的解析
PageInfo InformationExtract::getInfoFromRichText(Paper* paper)
{
    PageInfo pageInfo;
    pageInfo.pageHeight = paper->getHeight();
    pageInfo.pageWidth = paper->getWidth();
    pageInfo.lineDistance = paper->getLineDistance();
    pageInfo.letterDistanceScale = paper->getLetterDistanceScale();

    QTextFrame *rootFrame = paper->document()->rootFrame();
    QTextTable *table;
    QVector<BlockInfo> blockInfoContain;
    QTextBlock block;

    //得到框架信息,用于设置页边距
    QTextFrameFormat frameFormat = rootFrame->frameFormat();
    pageInfo.pageMargin.leftMargin = frameFormat.leftMargin();
    pageInfo.pageMargin.rightMargin = frameFormat.rightMargin();
    pageInfo.pageMargin.topMargin = frameFormat.topMargin();
    pageInfo.pageMargin.bottomMargin = frameFormat.bottomMargin();

    //遍历一个框架，根框架，迭代器取得其中的文本块
    for(QTextFrame::iterator it = rootFrame->begin(); !it.atEnd(); it++)
    {
        table = (QTextTable*)it.currentFrame();
        block = it.currentBlock();
        QVector<FragmentInfo> fragmentInfoContain;

        //取得文本块格式
        QTextBlockFormat blockFormat = block.blockFormat();
        BlockInfo blockInfo;

        //得到的都是0！！！！！！！！！！因为没有设置margin，距离感都是空格字符。。。
        blockInfo.leftMargin = blockFormat.leftMargin();
        blockInfo.rightMargin = blockFormat.rightMargin();
        blockInfo.topMargin = blockFormat.topMargin();
        blockInfo.bottomMargin = blockFormat.bottomMargin();
        blockInfo.lineHeight = blockFormat.lineHeight();
        blockInfo.indent = blockFormat.indent();              //缩进，后面的编写可能会遇到

        if(!block.text().isEmpty()&&!table)
        {
             //遍历文本块中的文本片段,文本输出主要用到的信息
            for(QTextBlock::iterator it = block.begin(); !it.atEnd(); it++)
            {
                //判断是否是图片资源
                QTextFragment fragment = it.fragment();
                FragmentInfo fragmentInfo;
                QTextCharFormat charFmt = fragment.charFormat();
                QTextImageFormat imageFmt = fragment.charFormat().toImageFormat();

                //由于QTextEdit本身对其进行转变,表格后面的文本格式为空
                if(charFmt.fontFamily().isEmpty())
                {
                    charFmt.setFontFamily("宋体");
                    charFmt.setFontPointSize(11);
                }

                if(imageFmt.isValid()) {
                    //提取图片
                    fragmentInfo.isImage = true;
                    fragmentInfo.isTable = false;         
                    qDebug() << imageFmt.name();
                    QString imagePath = imageFmt.name().right(imageFmt.name().size()-7);
                    // /*windows文件格式,linux删掉
                    imagePath.replace(1,1,':');
                    imagePath.insert(2,'/');
                    //  */
                    qDebug() << imagePath;
                    fragmentInfo.imagePath = imagePath;

                    fragmentInfo.imageNum = imageNum;   //图片ID计数
                    fragmentInfo.height = imageFmt.height();
                    fragmentInfo.width = imageFmt.width();
                    imageNum++;
                    qDebug() << "这是一张图片";
                }
                else
                {
                    fragmentInfo.fontFamily = charFmt.fontFamily();       //提取字体
                    fragmentInfo.size = charFmt.fontPointSize();          //提取字号
                    //得到文本段格式
                    fragmentInfo.isTable = false;
                    fragmentInfo.isImage = false;                         //不是图片     
                    fragmentInfo.italic = charFmt.fontItalic();           //是否斜体
                    fragmentInfo.underLine = charFmt.fontUnderline();     //是否下划线
                    fragmentInfo.bold = charFmt.fontWeight();             //是否加粗
                    fragmentInfo.color = convertColor(charFmt.foreground().color().red(),charFmt.foreground().color().green(),charFmt.foreground().color().blue());
                    fragmentInfo.text = fragment.text();                  //文本内容
                    qDebug() << "取得blockInfo内文本段 " << fragmentInfo.text;
                    qDebug() << "block文本格式" << charFmt.fontFamily();
                    qDebug() << "字体大小" << fragmentInfo.size;
                }
                fragmentInfoContain.append(fragmentInfo);
                blockInfo.empty = false;
                blockInfo.fragmentInfoContain = fragmentInfoContain;
            }
         }
         else if(block.text().isEmpty()&&table)
         {
            QTextTableFormat tableFormat = table->format();
            QVector<QTextLength> widthes = tableFormat.columnWidthConstraints();
            QTextTableCell cell;
            FragmentInfo fragmentInfo;

            for(int i=0; i<table->rows(); i++)
                for(int j=0; j<table->columns(); j++)
                {
                    cell = table->cellAt(i, j);
                    //填充空单元格
                    if(cell.begin().currentBlock().begin().atEnd())
                    {
                        fragmentInfo.isImage = false;
                        fragmentInfo.isTable = true;
                        fragmentInfo.text.clear();                           //空内容
                        fragmentInfo.cellWidth = widthes[j].rawValue();      //表格宽度
                        fragmentInfo.cellPosition.col = j;                   //行列位置
                        fragmentInfo.cellPosition.row = i;
                        fragmentInfoContain.append(fragmentInfo);
                        blockInfo.empty = false;
                        blockInfo.fragmentInfoContain = fragmentInfoContain;
                        qDebug() << "这是一个空单元格";
                    }

                    //表格内初始字体与全局字体一致，在这里将其初始化
                    for(QTextBlock::iterator it = cell.begin().currentBlock().begin(); !it.atEnd(); it++)
                    {
                        QTextFragment fragment = it.fragment();               
                        fragmentInfo.isTable = true;
                        QTextCharFormat charFmt = fragment.charFormat();
                        //得到表格内文本段格式
                        fragmentInfo.isImage = false;                         //不是图片
                        fragmentInfo.fontFamily = charFmt.fontFamily();       //提取字体
                        fragmentInfo.size = charFmt.fontPointSize();          //提取字号
                        fragmentInfo.italic = charFmt.fontItalic();           //是否斜体
                        fragmentInfo.underLine = charFmt.fontUnderline();     //是否下划线
                        fragmentInfo.bold = charFmt.fontWeight();             //是否加粗
                        fragmentInfo.color = convertColor(charFmt.foreground().color().red(),charFmt.foreground().color().green(),
                                                          charFmt.foreground().color().blue());
                        fragmentInfo.text = fragment.text();     //文本内容
                        fragmentInfo.cellWidth = widthes[j].rawValue();      //表格宽度
                        fragmentInfo.cellPosition.col = j;                   //行列位置
                        fragmentInfo.cellPosition.row = i;
                        fragmentInfoContain.append(fragmentInfo);
                        blockInfo.empty = false;
                        blockInfo.fragmentInfoContain = fragmentInfoContain;
                    }
                }
         }
         else
         {
            blockInfo.empty = true;
            qDebug() << "这是一个空block";
         }
         blockInfoContain.append(blockInfo);
         block = block.next();
    }
    pageInfo.blockInfoContain = blockInfoContain;
    return pageInfo;
}

//创建文档
void InformationExtract::writeToOFD(QString filename)
{
    OFDProcess *ofd = new OFDProcess(filename);
    ofd->createOFD(pageInfoContain);
    this->deleteLater();
}

QString InformationExtract::convertColor(int x, int y, int z)
{
    return QString::number(x).append(" ").append(QString::number(y)).append(" ").append(QString::number(z));
}

