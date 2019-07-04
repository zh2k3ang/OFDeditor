#ifndef THREAD_GETPAGETEXT_H
#define THREAD_GETPAGETEXT_H
#include "ofdprocess.h"
#include "paper.h"
#include <QThread>
#include <QMetaType>

class Thread_GetPageText : public QThread
{
    Q_OBJECT
public:
    Thread_GetPageText(Paper* p, int i);

    PageInfo getPageInfo();
protected:
    void run();
private:
    Paper* paper;
    PageInfo pageInfo;
    int number;
signals:
    void hasDone(PageInfo);
};

#endif // THREAD_GETPAGETEXT_H
