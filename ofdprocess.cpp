#include "ofdprocess.h"

OFDProcess::OFDProcess(QString filename)
{
    initDocInfo();
 //   initPageInfo();
 //   initTextInfo();

    file_name = filename;
    file_path = "./";
}

//初始化文档信息
void OFDProcess::initDocInfo()
{
    doc_info.author = "Zhang Kangkang";
    doc_info.creator = "OFD Editor";
    doc_info.creatorVersion = "1.0";
    doc_info.title = "Unknown";
    doc_info.uuid = QUuid::createUuid();
}

//初始化页面信息
void OFDProcess::initPageInfo()
{

}

//初始化文本信息
void OFDProcess::initTextInfo(TextInfo* textInfo)
{
    textInfo->size = "6";
    textInfo->boundary.dy = 0.0;
    textInfo->boundary.dx = 0.0;
    textInfo->boundary.x = 0.0;
    textInfo->boundary.y = 0.0;
    textInfo->bold = "400";
    textInfo->italic = "false";
    textInfo->underLine = "false";
    textInfo->color = "0 0 0";
}

//字体ID表，初始化一个100容量的一维数组，第一位为保存的字体总数
QString OFDProcess::findFontID(QString fontFamilay)
{
    for(int i=1; i<=fontMap[0].toInt(); i++)
    {
        if(fontMap[i] == fontFamilay)
            return QString::number(i);    //将int转换为QString类型
    }

    return QString::number(1);
}

//移动图片资源
void OFDProcess::copyImage(QString imagePath, int i)
{
    //bug：如果直接放图片复制会失败,要先输入文字后才能复制图片
    if(QFile::copy(imagePath, file_path+"Doc_0/Res/image_"+QString::number(i)+"."+imagePath.right(3)))
        qDebug() << "我复制了一张图片" << i;
    else
    {
       qDebug() << "复制出错" << i;
       qDebug() << imagePath;
       qDebug() << file_path+"Doc_0/Res/image_"+QString::number(i)+"."+imagePath.right(3);
    }

}

//创建OFD文档
bool OFDProcess::createOFD(QVector<PageInfo> pageInfoContain)
{
    //注意文件和目录的创建顺序！！！！
    QDir dir;
    dir.mkdir(file_path+"Doc_0");
    dir.mkdir(file_path+"Doc_0/Res");
    //文本转换必须在创建文档架构之前，但是文档结构没有创建，图片路径无法使用
    transformText(pageInfoContain);
    createOFD_xml();

    createDocument_xml();
    createPublicRes_xml();
    createDocumentRes_xml();

    //按实际情况创建页面文件夹
    dir.mkdir(file_path+"Doc_0/Pages");
    int i = 0;
    for(QVector<OFDPageInfo>::iterator it = ofdPageInfoContain.begin(); it!=ofdPageInfoContain.end(); it++)
    {
        qDebug() << "我在创建页面";
        dir.mkdir(file_path+"Doc_0/Pages/Page_"+QString::number(i));
        createContent_xml(i, *it);
        i++;
    }
    //创建OFD文档结构
    createZip();

    //删除临时文件，防止下次出错
    QFile::remove("OFD.xml");
    DeleteDirectory("Doc_0");
    return true;
}

//创建OFD.xml
bool OFDProcess::createOFD_xml()
{

    QFile file(file_path+"OFD.xml");
    QString uuid_QString = doc_info.uuid.toString();                   //转换为QString类型
    for(int i=0; i<uuid_QString.length(); i++)    //去掉多余的字符
    {
        if(uuid_QString.at(i)=='{'||uuid_QString.at(i)=='}'||uuid_QString.at(i)=='-')
        {
            uuid_QString.remove(i, 1);
            i--;
        }
    }

    if(!file.open(QFile::WriteOnly|QFile::Text)) return false;
    writer.setDevice(&file);                                           //将设备加载到流中
    writer.setAutoFormatting(true);                                    //自动缩进
    writer.setAutoFormattingIndent(2);                                 //缩进两格

    writer.writeStartDocument();                                       //文档开始
    writer.writeNamespace("http://www.ofdspec.org", "ofd");            //命名空间
    writer.writeStartElement("ofd:OFD");                            //OFD标签开始
    writer.writeAttribute("Version", "1.0");
    writer.writeAttribute("DocType", "OFD");

    writer.writeStartElement("ofd:DocBody");                        //DocBody标签开始
    writer.writeStartElement("ofd:DocInfo");                       //DocInfor标签开始

    writer.writeTextElement("ofd:Title", doc_info.title);
    writer.writeTextElement("ofd:DocID", uuid_QString);
    writer.writeTextElement("ofd:Auther", doc_info.author);
    writer.writeTextElement("ofd:Creator", doc_info.creator);
    writer.writeTextElement("ofd:CreatorVersion", doc_info.creatorVersion);

    writer.writeEndElement();                                          //DocInfo标签结束
    writer.writeTextElement("ofd:DocRoot", "Doc_0/Document.xml");

    writer.writeEndElement();                                          //DocBody标签开始
    writer.writeEndElement();                                          //OFD标签结束
    writer.writeEndDocument();                                         //文档结束

    file.close();

    return true;
}

bool OFDProcess::createDocument_xml()
{
    QFile file(file_path+"Doc_0/Document.xml");
    if(!file.open(QFile::WriteOnly|QFile::Text)) return false;

    writer.setDevice(&file);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    writer.writeStartDocument();
    writer.writeNamespace("http://www.ofdspec.org", "ofd");
    writer.writeStartElement("ofd:Document");                       //Document标签开始
    writer.writeStartElement("ofd:CommonData");                     //CommonData标签开始

 //   writer.writeTextElement("ofd:MaxUnitID", "200");
    writer.writeStartElement("ofd:PageArea");                       //PageArea标签开始
    writer.writeTextElement("ofd:PhysicalBox", A4page_page_PhysicalBox);       //默认的物理区大小，取决于页面的物理区大小
    writer.writeEndElement();                                          //PageArea标签结束
    writer.writeTextElement("ofd:PublicRes", "PublicRes.xml");
    writer.writeTextElement("ofd:DocumentRes", "DocumentRes.xml");
    writer.writeEndElement();                                          //CommonData标签结束

    writer.writeStartElement("ofd:Pages");                          //pages标签开始

    int i = 0;
    for(QVector<OFDPageInfo>::iterator it = ofdPageInfoContain.begin(); it!=ofdPageInfoContain.end(); it++)
    {
        writer.writeEmptyElement("ofd:Page");
        writer.writeAttribute("BaseLoc", "Pages/Page_"+QString::number(i)+"/Content.xml");
        writer.writeAttribute("ID", QString::number(100+i));                                  //ID号，每一页唯一
        i++;
    }

    writer.writeEndElement();                                          //Pages标签结束

    writer.writeStartElement("ofd:Action");                         //Action标签开始
    writer.writeEndElement();                                          //Action标签结束

    writer.writeStartElement("ofd:Bookmark");                       //BookMark标签开始
    writer.writeEndElement();                                          //BookMark标签结束
    writer.writeEndElement();                                          //Document标签结束
    writer.writeEndDocument();

    file.close();
    return true;

}


//定义字体的ID号
bool OFDProcess::createPublicRes_xml()
{
    QFile file(file_path+"Doc_0/PublicRes.xml");
    if(!file.open(QFile::WriteOnly|QFile::Text)) return false;

    writer.setDevice(&file);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    writer.writeStartDocument();
    writer.writeNamespace("http://www.ofdspec.org", "ofd");
    writer.writeStartElement("ofd:Res");                            //Res标签开始
    writer.writeAttribute("BaseLoc", "Res");
    writer.writeStartElement("ofd:Fonts");                          //Fonts标签开始

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "黑体");
    writer.writeAttribute("ID", "1");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "隶书");
    writer.writeAttribute("ID", "2");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "等线 Light");
    writer.writeAttribute("ID", "3");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "等线");
    writer.writeAttribute("ID", "4");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "楷体");
    writer.writeAttribute("ID", "5");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "方正舒体");
    writer.writeAttribute("ID", "6");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "方正姚体");
    writer.writeAttribute("ID", "7");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "新宋体");
    writer.writeAttribute("ID", "8");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "微软雅黑 Light");
    writer.writeAttribute("ID", "9");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "微软雅黑");
    writer.writeAttribute("ID", "10");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "幼圆");
    writer.writeAttribute("ID", "11");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "宋体");
    writer.writeAttribute("ID", "12");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "华文隶书");
    writer.writeAttribute("ID", "13");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "华文行楷");
    writer.writeAttribute("ID", "14");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "华文细黑");
    writer.writeAttribute("ID", "15");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "华文琥珀");
    writer.writeAttribute("ID", "16");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "华文楷体");
    writer.writeAttribute("ID", "17");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "华文新魏");
    writer.writeAttribute("ID", "18");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "华文彩云");
    writer.writeAttribute("ID", "19");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "华文宋体");
    writer.writeAttribute("ID", "20");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "华文仿宋");
    writer.writeAttribute("ID", "21");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "华文中宋");
    writer.writeAttribute("ID", "22");

    writer.writeEmptyElement("ofd:Font");                           //Font标签开始
    writer.writeAttribute("FontName", "仿宋");
    writer.writeAttribute("ID", "23");

    writer.writeEndElement();                                          //Fonts标签结束
    writer.writeEndElement();                                          //Res标签结束
    writer.writeEndDocument();

    file.close();
    return true;
}

bool OFDProcess::createDocumentRes_xml()
{
    QFile file(file_path+"Doc_0/DocumentRes.xml");
    if(!file.open(QFile::WriteOnly|QFile::Text)) return false;

    writer.setDevice(&file);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    writer.writeStartDocument();
    writer.writeNamespace("http://www.ofdspec.org", "ofd");
    writer.writeStartElement("ofd:Res");                            //Res标签开始
    writer.writeAttribute("BaseLoc", "Res");
    writer.writeStartElement("ofd:MultiMedias");                    //MultiMedias标签开始

    //循环找出所有的图片资源
    for(QVector<OFDPageInfo>::iterator it = ofdPageInfoContain.begin(); it!=ofdPageInfoContain.end(); it++)
    {
        QVector<OFDLineInfo> lineInfoContain = it->lineInfoContain;
        for(QVector<OFDLineInfo>::iterator it = lineInfoContain.begin(); it!=lineInfoContain.end(); it++)
        {
            QVector<TextInfo> textInfoContain = it->textInfoContain;
            for(QVector<TextInfo>::iterator it = textInfoContain.begin(); it!=textInfoContain.end(); it++)
            {
                qDebug() << "我在找图片";
                if(it->isImage)
                {
                    qDebug() << "找到了";
                    qDebug() << "ID: " << it->imageNum;
                    //循环写入multimadia和mediafile标签
                    writer.writeStartElement("ofd:MultiMedia");                    //MultiMedia标签开始
                    writer.writeAttribute("ID", QString::number(100+it->imageNum));
                    writer.writeAttribute("Type", "Image");
                    writer.writeTextElement("ofd:MediaFile", "image_"+QString::number(it->imageNum)+"."+it->imagePath.right(3));  //图片名称
                    writer.writeEndElement();                                      //MultiMedia标签结束
                }
            }
        }
    }

    writer.writeEndElement();                                       //MultiMedias标签结束
    writer.writeEndElement();                                       //Res标签结束
    writer.writeEndDocument();
    file.close();
    return true;
}


bool OFDProcess::createZip()
{
    QZipWriter zip_writer(file_name);
    qDebug() << file_name;
    //将目录添加到ZIP文件中
    zip_writer.addDirectory("Doc_0/Pages");
    zip_writer.addDirectory("Doc_0/Res");
    zip_writer.addDirectory("Doc_0/Signs");

    //添加文件
    QFile file(file_path+"OFD.xml");
    if(!file.open(QIODevice::ReadOnly)) return false;
    zip_writer.addFile("OFD.xml", file.readAll());
    file.close();

    file.setFileName(file_path+"Doc_0/Document.xml");
    if(!file.open(QIODevice::ReadOnly)) return false;
    zip_writer.addFile("Doc_0/Document.xml", file.readAll());
    file.close();

    file.setFileName(file_path+"Doc_0/PublicRes.xml");
    if(!file.open(QIODevice::ReadOnly)) return false;
    zip_writer.addFile("Doc_0/PublicRes.xml", file.readAll());
    file.close();

    file.setFileName(file_path+"Doc_0/DocumentRes.xml");
    if(!file.open(QIODevice::ReadOnly)) return false;
    zip_writer.addFile("Doc_0/DocumentRes.xml", file.readAll());
    file.close();

    //循环找出所有的图片资源并写入ofd文档中
    for(QVector<OFDPageInfo>::iterator it = ofdPageInfoContain.begin(); it!=ofdPageInfoContain.end(); it++)
    {
        QVector<OFDLineInfo> lineInfoContain = it->lineInfoContain;
        for(QVector<OFDLineInfo>::iterator it = lineInfoContain.begin(); it!=lineInfoContain.end(); it++)
        {
            QVector<TextInfo> textInfoContain = it->textInfoContain;
            for(QVector<TextInfo>::iterator it = textInfoContain.begin(); it!=textInfoContain.end(); it++)
            {
                if(it->isImage)
                {
                    file.setFileName(file_path+"Doc_0/Res/image_"+QString::number(it->imageNum)+"."+it->imagePath.right(3));
                    if(!file.open(QIODevice::ReadOnly)) return false;
                    zip_writer.addFile("Doc_0/Res/image_"+QString::number(it->imageNum)+"."+it->imagePath.right(3), file.readAll());
                    file.close();
                }
            }
        }
    }

    int i = 0;
    for(QVector<OFDPageInfo>::iterator it = ofdPageInfoContain.begin(); it!=ofdPageInfoContain.end(); it++)
    {
        file.setFileName(file_path+"Doc_0/Pages/Page_"+QString::number(i)+"/Content.xml");
        if(!file.open(QIODevice::ReadOnly)) return false;
        zip_writer.addFile("Doc_0/Pages/Page_"+QString::number(i)+"/Content.xml", file.readAll());
        file.close();
        i++;
    }

    zip_writer.close();

    return true;
}
//遍历删除非空文件夹
bool OFDProcess::DeleteDirectory(QString path)
{
    if (path.isEmpty())
        return false;

    QDir dir(path);
    if(!dir.exists())
        return true;

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo fi, fileList)
    {
        if (fi.isFile())
            fi.dir().remove(fi.fileName());
        else
            DeleteDirectory(fi.absoluteFilePath());
    }
    return dir.rmpath(dir.absolutePath());
}

//将容器内保存的数据插入到页面中！！重点内容
bool OFDProcess::createContent_xml(int i, OFDPageInfo ofdPageInfo)
{
    QFile file((file_path+"Doc_0/Pages/Page_%1/Content.xml").arg(QString::number(i)));
    if(!file.open(QFile::WriteOnly|QFile::Text)) return false;

    writer.setDevice(&file);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    writer.writeStartDocument();
    writer.writeNamespace("http://www.ofdspec.org", "ofd");
    writer.writeStartElement("ofd:Page");                           //Page标签开始
    writer.writeStartElement("ofd:Area");                           //Area标签开始
    writer.writeStartElement("ofd:PhysicalBox");                    //PhysicalBox
    writer.writeCharacters(ofdPageInfo.page_PhysicalBox);           //写入物理区大小
    writer.writeEndElement();                                       //PhysicalBox
    writer.writeEndElement();                                       //Area标签结束
    writer.writeStartElement("ofd:Content");                        //Content标签开始
    writer.writeStartElement("ofd:Layer");                          //Layer标签开始
    writer.writeAttribute("Type", "body");


    //将textInfoContain容器中的内容写入进文件
    QVector<OFDLineInfo> lineInfoContain = ofdPageInfo.lineInfoContain;
    for(QVector<OFDLineInfo>::iterator it = lineInfoContain.begin(); it!=lineInfoContain.end(); it++)
    {
        double lineHeight = it->lineHeight.toDouble();
        double lineDistance = it->lineDistance;
        QVector<TextInfo> textInfoContain = it->textInfoContain;
        for(QVector<TextInfo>::iterator it = textInfoContain.begin(); it!=textInfoContain.end(); it++)
        {
            //写入文本
            if(!it->isImage)
            {
                writer.writeStartElement("ofd:TextObject");                     //TextObject标签开始
        //      writer.writeAttribute("CTM", textInfo.CTM);                    //CTM  不使用CTM一样可以！！做了一下午的线代练习发现这个鬼真理。。。
                writer.writeAttribute("Boundary", it->boundary.str);
                qDebug() << "boundary" << it->boundary.str;

                writer.writeAttribute("Font", it->fontID);
                writer.writeAttribute("Size", it->size);
                writer.writeAttribute("Italic", it->italic);                    //斜体
                writer.writeAttribute("Weight", it->bold);
                writer.writeEmptyElement("ofd:FillColor");
                writer.writeAttribute("Value", it->color);
                writer.writeStartElement("ofd:TextCode");                       //TextCode标签开始
                writer.writeAttribute("X", it->X);
                writer.writeAttribute("Y", it->Y);
                writer.writeAttribute("DeltaX", it->DeltaX);
                writer.writeCharacters(it->text);
                qDebug() << "写入文本：" << it->text;
                writer.writeEndElement();                                          //TextCode标签结束  这俩标签肯定要写在循环里面啊！！！
                writer.writeEndElement();                                          //TextObject标签结束

                //绘制下滑线，图形对象
                if(it->underLine == "true")
                {
                    writer.writeStartElement("ofd:PathObject");
                    Boundary boundary;
                    boundary.x = it->boundary.x;
                    boundary.y = it->boundary.y+lineHeight;
                    boundary.dx = it->boundary.dx;
                    boundary.dy = 0.05;
                    boundary.str = calculateBoundary_str(boundary);
                    writer.writeAttribute("Boundary", boundary.str);
                    writer.writeTextElement("ofd:AbbreviatedData", "M 0 0.05 L "+QString::number(it->boundary.dx)
                                            +" 0.05 L "+QString::number(it->boundary.dx)+" 0 L 0 0 C");
                    writer.writeEndElement();
                }
                //绘制表格
                if(it->isTable)
                {
                    writer.writeStartElement("ofd:PathObject");
                    Boundary boundary;
                    boundary.x = it->boundary.x;
                    boundary.y = it->boundary.y-0.2;   //防止头顶被削
                    boundary.dx = it->boundary.dx;
                    boundary.dy = lineHeight + lineDistance;
                    boundary.str = calculateBoundary_str(boundary);
                    writer.writeAttribute("Boundary", boundary.str);
                    writer.writeTextElement("ofd:AbbreviatedData", "M 0 0 L "+QString::number(it->boundary.dx)+" 0 L "
                                            +QString::number(it->boundary.dx)+" "+QString::number(lineHeight+lineDistance)
                                            +" L 0 "+QString::number(lineHeight+lineDistance)+" C");
                    writer.writeEndElement();
                }
            }
            //写入图片
            else
            {
                writer.writeEmptyElement("ofd:ImageObject");                      //ImageObject
                writer.writeAttribute("ResourceID", QString::number(it->imageNum+100));
                writer.writeAttribute("Boundary", it->boundary.str);
                //图像需用CTM矩阵缩放
                writer.writeAttribute("CTM", QString::number(CTImage*it->width)+" 0 0 "+QString::number(CTImage*it->height)+" 0 0");
            }

        }
        qDebug() << "结束一行！！！！！！！！！";

    }

    writer.writeEndElement();                                          //Layer标签结束
    writer.writeEndElement();                                          //Content标签结束
    writer.writeEndElement();                                          //Page标签结束
    writer.writeEndDocument();

    file.close();
    return true;
}

QString OFDProcess::calculateDeltaX(TextInfo textInfo)
{
    //计算DeltaX，这里字符偏移距离为每个字符的长度，及每个字符紧挨着，若改变字间距，则可以调整计算这个参数
    QString Deltax;                                  //Deltax为字符之间的偏移距离，共有总字符数-1个
    for(int i=0; i<textInfo.text.length()-1; i++)
    {
        //找出字符串里的字母和空格，大小*CTLetter
        if(textInfo.text.at(i)>31&&textInfo.text.at(i)<128)
            Deltax.append(QString::number(textInfo.size.toDouble()*CTLetter*textInfo.dxScale)).append(" ");
        else
            Deltax.append(QString::number(textInfo.size.toDouble()*textInfo.dxScale)).append(" ");
    }
    Deltax.chop(1);       //去掉最后一个空格
    return Deltax;
}

double OFDProcess::calculateBoundary_dx(TextInfo textInfo)
{
    textInfo.boundary.dx = 0.0;
    for(int i=0; i<textInfo.text.length(); i++)   //转换文本段里的英文字符
    {
        if(textInfo.text.at(i)>31&&textInfo.text.at(i)<128)
            textInfo.boundary.dx = textInfo.boundary.dx+textInfo.size.toDouble()*textInfo.dxScale*CTLetter;
        else
            textInfo.boundary.dx = textInfo.boundary.dx+textInfo.size.toDouble()*textInfo.dxScale;
    }
    return textInfo.boundary.dx;
}

QString OFDProcess::calculateBoundary_str(Boundary boundary)
{
    boundary.str = QString::number(boundary.x).append(" ").append(QString::number(boundary.y)).append(" ")
            .append(QString::number(boundary.dx)).append(" ").append(QString::number(boundary.dy));
    return boundary.str;
}

//将一行存入容器后，重新设置行高，并重新设置boundary.dy和boundary.str
void OFDProcess::resetLineHeight(OFDLineInfo* lineInfo)
{
    lineInfo->lineHeight = QString::number(lineInfo->textInfoContain.begin()->boundary.dy);
    double y = lineInfo->textInfoContain.begin()->boundary.y;        //保存行高
    for(QVector<TextInfo>::iterator it = lineInfo->textInfoContain.begin();it!=lineInfo->textInfoContain.end(); it++)
    {
        if(it->boundary.dy > lineInfo->lineHeight.toDouble())
            lineInfo->lineHeight = QString::number(it->boundary.dy);
    }
    for(QVector<TextInfo>::iterator it = lineInfo->textInfoContain.begin();it!=lineInfo->textInfoContain.end(); it++)
    {

        if(it->isImage)         //图片通过调整boundary.y使下方对齐
        {
            it->boundary.y = y + lineInfo->lineHeight.toDouble() - it->boundary.dy - BaseLine;    //baseLine防止上方被削一块
        }
        else              //文字通过调整Y使文本下方对齐
        {
            it->boundary.dy = lineInfo->lineHeight.toDouble();
            it->Y = QString::number(lineInfo->lineHeight.toDouble()- BaseLine);    //使文本下方对齐， baseLine防止下方被削一块
        }
        it->boundary.str = calculateBoundary_str(it->boundary);
    }
    qDebug() << "行高：" << lineInfo->lineHeight;
}

//此函数代码冗余严重，文本结构混乱，带修缮
QVector<OFDLineInfo> OFDProcess::transformBlockInfo(PageInfo *pageInfo)
{
    QVector<BlockInfo> blockInfoContain = pageInfo->blockInfoContain;
    QVector<OFDLineInfo> lineInfoContain;
    OFDLineInfo lineInfo;
    TextInfo textInfo;

    FragmentInfo lastFrag;
    lastFrag.isTable = false;
    initTextInfo(&textInfo);

    textInfo.dxScale = pageInfo->letterDistanceScale;
    textInfo.boundary.y = CTMargin*pageInfo->pageMargin.topMargin;      //不断改变
    qDebug() << "初始Y坐标"<<  textInfo.boundary.y;
    lineInfo.lineHeight = textInfo.size;
    lineInfo.lineDistance = pageInfo->lineDistance*CTLineDistance;

    for(QVector<BlockInfo>::iterator it = blockInfoContain.begin(); it!=blockInfoContain.end(); it++)
    {
        int lastCell = 0;
        textInfo.boundary.x = CTMargin*pageInfo->pageMargin.leftMargin;   //每个block中的boundary.x起始重设到左原点位置
        textInfo.boundary.dx = 0.0;      //每个block中的boundary.dx起始重设为0
        qDebug() << "文本段开始Y坐标"<<  textInfo.boundary.y;
        //完成文本段的解析
        if(it->empty)     //文本块为空，输出空行，即整个textInfo垂直位置向下平移一行
        {
            qDebug() << "获得一个空的block";
            textInfo.boundary.y = textInfo.boundary.y+lineInfo.lineHeight.toDouble()+lineInfo.lineDistance;      //实现空行
            qDebug() << "空行后Y坐标"<<  textInfo.boundary.y;
        }
        else
        {
            //取得文本段中的信息，包括文本内容和格式,并装入容器中
            QVector<FragmentInfo> fragmentInfoContain = it->fragmentInfoContain;
            for(QVector<FragmentInfo>::iterator itf = fragmentInfoContain.begin();itf != fragmentInfoContain.end(); itf++)
            {
                lastFrag = *itf;
                qDebug() << lastFrag.isTable;
               //判断是否图片
               if(!itf->isImage)
               {
                   qDebug() << "获得block内文本段：" << itf->text;
                   textInfo.isImage = false;
                   textInfo.isTable = false;
                   textInfo.text = itf->text;
                   textInfo.size = QString::number(CTFont*itf->size);           //数字转化为字符串
                   textInfo.fontID = findFontID(itf->fontFamily);
                   textInfo.Y = textInfo.size;            //Y为边界到字符的底部位置，暂且为字符的高度

                   qDebug() << "字体大小" << textInfo.size;

                   textInfo.X = "0";                       //暂且为0.外接矩形的水平边界位置
                   textInfo.DeltaX = calculateDeltaX(textInfo);
                   textInfo.boundary.x = textInfo.boundary.x+textInfo.boundary.dx;   //之前的x位置加之前的偏移距离
                   textInfo.boundary.dx = calculateBoundary_dx(textInfo);
                   textInfo.boundary.dy = textInfo.size.toDouble();

                   textInfo.bold = (itf->bold == 50) ? QString::number(400) : QString::number(500);    //加粗
                   textInfo.italic = itf->italic ? "true" : "false";
                   textInfo.underLine = itf->underLine ? "true" : "false";
                   textInfo.color = itf->color;

                   //长度检测  文本长度剪掉边距和这一块文本的开始位置
                   while(textInfo.boundary.dx > pageInfo->pageWidth*CTView - CTMargin*pageInfo->pageMargin.rightMargin-textInfo.boundary.x)
                   {
                       double onebyone = 0.0;
                       for(int i=0; i < textInfo.text.length(); i++)
                       {
                           if(textInfo.text.at(i)>31&&textInfo.text.at(i)<128)       //处理字母和数字的宽度问题
                               onebyone = onebyone+textInfo.size.toDouble()*textInfo.dxScale*CTLetter;
                           else
                               onebyone = onebyone+textInfo.size.toDouble()*textInfo.dxScale;

                           if(onebyone > (pageInfo->pageWidth*CTView-CTMargin*pageInfo->pageMargin.rightMargin-textInfo.boundary.x))
                           {
                               //对字符串进行截取，且格式相同
                               TextInfo textInfo_left = textInfo;                   //截取应该在本行的字符
                               textInfo_left.text = textInfo.text.left(i);
                               TextInfo textInfo_right = textInfo;
                               textInfo_right.text = textInfo.text.right(textInfo.text.length()-i);   //截取应该换行的字符

                               textInfo_left.boundary.dx = calculateBoundary_dx(textInfo_left);  //重新计算偏移距离
                               textInfo_left.DeltaX = calculateDeltaX(textInfo_left);             //重新计算Deltax

                               lineInfo.textInfoContain.append(textInfo_left);   //将本段文字加入行
                               resetLineHeight(&lineInfo);             //重设行高
                               lineInfoContain.append(lineInfo);                 //由于本段文字沾满一行，所以将lineInfo加入行容器
                               lineInfo.textInfoContain.clear();    //换行清容器
                               qDebug() << "存入行容器的字符串"<< textInfo_left.text;

                               //右面一段全部改变,这里不考虑单个字符的垂直偏移距离
                               textInfo_right.boundary.x = CTMargin*pageInfo->pageMargin.leftMargin;        //左面回到原点；
                               textInfo_right.boundary.dx = calculateBoundary_dx(textInfo_right);           //外接矩形的偏移距离
                               textInfo_right.DeltaX = calculateDeltaX(textInfo_right);                     //重新计算Deltax
                               textInfo_right.boundary.y = textInfo_right.boundary.y + lineInfo.lineHeight.toDouble()+lineInfo.lineDistance;  //垂直方向改变
                               textInfo = textInfo_right;       //重新进入循环检测长度
                               break;  //退出循环！！
                           }
                       }
                       qDebug() << "我在这";
                   }
                   //对表格的处理
                   if(itf->isTable)
                   {
                       textInfo.isTable = true;
                       textInfo.boundary.dx = CTTable*itf->cellWidth;
                       if(itf->cellPosition.row > lastCell)
                       {
                           //下一行单元格重置
                           textInfo.boundary.x = CTMargin*pageInfo->pageMargin.leftMargin;        //左面回到原点；
                           textInfo.boundary.y = textInfo.boundary.y + lineInfo.lineHeight.toDouble()+lineInfo.lineDistance;  //垂直方向改变
                       }

                       textInfo.bold = (itf->bold == 50) ? QString::number(400) : QString::number(500);    //加粗
                       textInfo.italic = itf->italic ? "true" : "false";
                       textInfo.underLine = itf->underLine ? "true" : "false";
                       textInfo.color = itf->color;
                       lastCell = itf->cellPosition.row;
                   }                
               }
               //对图片的处理
               else
               {
                   qDebug() << "获得一张图片";
                   //将图片资源移动到OFD资源文件夹下
                   textInfo.isImage = true;
                   textInfo.isTable = false;
                   textInfo.imagePath = itf->imagePath;
                   textInfo.imageNum = itf->imageNum;
                   textInfo.width = itf->width;
                   textInfo.height = itf->height;
                   //图片中boundary的dx与dy与CTM的a和d参数相同
                   textInfo.boundary.x = textInfo.boundary.x+textInfo.boundary.dx;   //之前的x位置加之前的偏移距离
                   textInfo.boundary.dx = CTImage*textInfo.width;
                   textInfo.boundary.dy = CTImage*textInfo.height;
                   calculateBoundary_str(textInfo.boundary);
                   copyImage(textInfo.imagePath, textInfo.imageNum);
               }

               lineInfo.textInfoContain.append(textInfo);
               qDebug() << "存入时Y坐标"<<  textInfo.boundary.y;
               resetLineHeight(&lineInfo);              //重设行高
               qDebug() << "存入行容器的字符串"<< textInfo.text;
               qDebug() << "行距：" << lineInfo.lineDistance;
            }
            lineInfoContain.append(lineInfo);
            qDebug() << "一行结束前："<<textInfo.boundary.y;
            if(!lastFrag.isTable)
                textInfo.boundary.y = textInfo.boundary.y+lineInfo.lineHeight.toDouble()+lineInfo.lineDistance;         //一行结束后换行
            qDebug() << "一行结束后换行："<<textInfo.boundary.y;
            lineInfo.textInfoContain.clear();    //换行清容器
        }
    }
    return lineInfoContain;
}

//将从textprocess中的信息提取出来存放到ofd格式文本段的容器中
void OFDProcess::transformText(QVector<PageInfo> pageInfoContain)
{
    //将pageInfoContain中的信息转换存入ofdPageInfoContain中
    for(QVector<PageInfo>::iterator it = pageInfoContain.begin(); it!=pageInfoContain.end(); it++)
    {
        qDebug() << "行高是否取得：" << it->lineDistance;
        OFDPageInfo page_info;                   //页面信息
        page_info.leftMargin = QString::number(CTMargin*it->pageMargin.leftMargin);        //别忘了类型转换！！！
        page_info.rightMargin = QString::number(CTMargin*it->pageMargin.rightMargin);
        page_info.topMargin = QString::number(CTMargin*it->pageMargin.topMargin);
        page_info.bottomMargin = QString::number(CTMargin*it->pageMargin.bottomMargin);
        page_info.page_PhysicalBox = transformPhysicalBox(*it);
        //遍历pageInfo中的文本块信息
        page_info.lineInfoContain = transformBlockInfo(it);
        ofdPageInfoContain.append(page_info);
    }
}

QString OFDProcess::transformPhysicalBox(PageInfo pageInfo)
{
    QString physicalBox = "0 0 ";
    physicalBox.append(QString::number(pageInfo.pageWidth*CTView)).append(" ").append(QString::number(pageInfo.pageHeight*CTView));
    return physicalBox;
}

