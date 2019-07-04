#ifndef OFDPROCESS_H
#define OFDPROCESS_H

#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QDir>
#include <QString>
#include <QUuid>
#include <QMessageBox>
#include <QtGui\5.8.0\QtGui\private\qzipwriter_p.h>
#include <QMainWindow>
#include <QTextDocument>
#include <QVector>
#include <QTextTable>
#include <QTextTableCell>
#include <QTextLength>
#include <QDebug>

//页面大小
#define A4page_page_PhysicalBox "0 0 210 297"
#define A4Tpage_page_PhysicalBox "0 0 297 210"
#define A5page_page_PhysicalBox "0 0 148 210"
#define A5Tpage_page_PhysicalBox "0 0 210 148"
#define B5page_page_PhysicalBox "0 0 210 276"
#define B5Tpage_page_PhysicalBox "0 0 276 210"

const double CTFont = 0.343;           //显示大小与输出到ofd文字之间的系数
const double CTMargin = 0.2058;           //显示大小与输出到ofdmargin之间的系数
const double CTLetter = 0.531;          //英文字符的偏移距离系数
const double CTView = 0.2;             //paper显示与所得的缩放比例
const double CTLineDistance = 0.22;
const double CTImage = 0.2;             //图片显示系数
const double CTTable = 0.215;            //表格宽度转换参数
const double BaseLine = 1;       //防止文字最下方上方被削掉一点

typedef struct CellPosition
{
    int row;
    int col;
}CellPosition;

typedef struct FragmentInfo
{
    QString fontFamily;
    double size;
    bool underLine;
    int bold;
    bool italic;
    QString color;
    QString text;

    bool isImage;
    QString imagePath;
    int imageNum;
    double height;
    double width;

    bool isTable;
    CellPosition cellPosition;
    double cellWidth;
    double celleHeight;
    int tablePosition;
}Fragment;

typedef struct BlockInfo
{
    double leftMargin;     //blockInfo里的margin可以用来设置段落信息
    double rightMargin;
    double topMargin;
    double bottomMargin;
    double lineHeight;
    int indent;             //缩进
    bool empty;             //确定文本块是否为空，为换行打下基础
    QVector<FragmentInfo> fragmentInfoContain;
}BlockInfo;

//表格信息
typedef struct TableInfo
{

}TableInfo;

typedef struct pageMargin
{
    double leftMargin;
    double rightMargin;
    double bottomMargin;
    double topMargin;
}PageMargin;

//QDocument文本信息结构体
typedef struct PageInfo
{
    int number;
    double pageHeight;
    double pageWidth;
    double lineDistance;        //行距
    double letterDistanceScale;  //字距比例
    PageMargin pageMargin;
    QVector<BlockInfo> blockInfoContain;
    QVector<TableInfo> tableInfoContain;
}PageInfo;

//OFD文档信息结构体
typedef struct DocInfo
{
    QString title;                        //文档标题
    QString author;                       //文档作者
    QString creator;                      //编辑器
    QString creatorVersion;               //编辑器版本
    QUuid uuid;                           //全球唯一标识符uuid
}DocInfo;

//文本信息结构体 都是字符串类型，为了XML的写入
typedef struct Boundary
{
    double x;
    double y;
    double dx;
    double dy;
    QString str;
}Boudary;

typedef struct TextInfo
{
    QString type;                         //类型
    QString fontID;                       //字体，值为PublicRes.xml定义的字体ID号
    QString CTM;                          //CTM矩阵变换
    Boundary boundary;                     //边界
    QString size;                         //字体大小
    double dxScale;
 //   QTextDocument text;
    QString X;
    QString Y;
    QString DeltaX;
    QString text;

    QString underLine;
    QString italic;
    QString bold;
    QString color;

    bool isImage = false;
    bool isTable = false;
    QString imagePath;
    int imageNum;
    double height;
    double width;
}TextInfor;

typedef struct OFDImageInfo
{

}OFDImageInfo;

typedef struct OFDLineInfo
{
    QString lineHeight;
    double lineDistance;
    QVector<TextInfo> textInfoContain;
}OFDLineInfo;


//页面信息结构体
typedef struct OFDPageInfo
{
    QString page_PhysicalBox;             //页面物理区大小
    QString leftMargin;
    QString rightMargin;
    QString topMargin;
    QString bottomMargin;
    QVector<OFDLineInfo> lineInfoContain;
}OFDPageInfo;

class OFDProcess
{
public:
    OFDProcess(QString);

    void initDocInfo();                   //初始化文档信息
    void initPageInfo();                  //初始化页面信息
    void initTextInfo(TextInfo*);                  //初始化文本信息
    QString findFontID(QString);          //从字体ID表中找到ID

    bool createOFD(QVector<PageInfo>);                     //创建OFD文档
    bool createOFD_xml();                 //创建OFD.xml
    bool createDocument_xml();            //创建Document.xml
    bool createPublicRes_xml();           //创建PublicRes.xml
    bool createContent_xml(int, OFDPageInfo);             //创建Content.xml
    bool createDocumentRes_xml();         //创建DocumentRes.xml
    bool createZip();                     //创建ZIP，OFD文档的组织方式

    void setPath();                       //修改文档保存位置
    void setFileName();                   //修改文档名

    void setDocInfo();                    //修改文档信息
    void setPageInfo();                   //修改页面信息
    void setTextInfo();                   //修改文本信息

    QVector<OFDLineInfo> transformBlockInfo(PageInfo*);
    QString calculateX(FragmentInfo);             //计算TextCode X值QVector<BlockInfo> blockInfoContain
    QString calculateY(FragmentInfo);             //计算TextCode Y值
    QString calculateDeltaX(TextInfo);            //计算TextCode DeltaX值
    double calculateBoundary_dx(TextInfo);
    QString calculateBoundary_str(Boundary);          //计算TextObject boundary值
    void resetLineHeight(OFDLineInfo*);
    void transformText(QVector<PageInfo>);        //接口函数，从界面文件中接受文本数据
    QString transformPhysicalBox(PageInfo);
    void copyImage(QString, int);
    static bool DeleteDirectory(QString);

private:
    QXmlStreamWriter writer;              //XML写入流
    QXmlStreamReader reader;              //XML读取流
    QString file_path;                    //文档路径
    QString file_name;                    //文档名称
    DocInfo doc_info;                     //文档信息
    QVector<OFDPageInfo> ofdPageInfoContain;
    QVector<DocInfo>  docInfoContain;

    QString fontMap[100] = {"23", "黑体", "隶书", "等线 Light", "等线", "楷体", "方正舒体", "方正姚体",
                           "新宋体", "微软雅黑 Light", "微软雅黑", "幼圆", "宋体", "华文隶书", "华文行楷",
                           "华文细黑", "华文琥珀", "华文楷体", "华文新魏", "华文彩云", "华文宋体", "华文仿宋",
                           "华文中宋","仿宋"};
};

#endif // OFDPROCESS_H
