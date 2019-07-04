#ifndef INFORMATIONEXTRACT_H
#define INFORMATIONEXTRACT_H

#include <QTextDocument>
#include <QTextBlock>
#include <QTextBlockFormat>
#include <QTextTableFormat>
#include <QTextList>
#include <QTextListFormat>
#include <QTextFragment>
#include <QVector>
#include <QString>
#include <QTextEdit>
#include <QTextTableCell>
#include <QTextLength>
#include "ofdprocess.h"
#include "paper.h"

class InformationExtract:public QObject
{
    Q_OBJECT                      //手动执行一次qmake
public:
    InformationExtract(QObject* parent=0);

    void RichTextToOFD(QVector<Paper*>, QString);
    static PageInfo getInfoFromRichText(Paper*);
    void writeToOFD(QString);
    static QString convertColor(int, int, int);
    static bool cmp(const PageInfo&, const PageInfo&);   //qSort
private:
    QVector<PageInfo> pageInfoContain;
    PageInfo temp_pageInfo;
    QString filename;
    int pageNum;
    static int imageNum;     //图片计数
protected:
    void run();
public slots:
    void receiveThread(PageInfo pageInfo);
};

#endif // INFORMATIONEXTRACT_H

