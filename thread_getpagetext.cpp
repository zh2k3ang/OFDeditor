#include "thread_getpagetext.h"
#include "informationextract.h"

Thread_GetPageText::Thread_GetPageText(Paper* p, int i)
    :paper(p), number(i)
{
    //将不识别的参数结构进行注册，让Qt能够识别。

    qRegisterMetaType<PageInfo>("PageInfo");//注册PVCI_CAN_OBJ类型
}

void Thread_GetPageText::run()
{
    pageInfo = InformationExtract::getInfoFromRichText(paper);
    pageInfo.number = number;
    qDebug() << "页号"<< pageInfo.number;
    qDebug() << "我在线程中";
    emit hasDone(pageInfo);        //提取信息完毕后发信号给InformationExtrct接受
    this->quit();    //结束线程
    this->deleteLater();   //删除对象
}

PageInfo Thread_GetPageText::getPageInfo()
{
    return pageInfo;
}
