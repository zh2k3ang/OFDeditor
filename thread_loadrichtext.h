#ifndef THREAD_LOADRICHTEXT_H
#define THREAD_LOADRICHTEXT_H

#include <QThread>
#include "richtextprocess.h"
class Thread_loadRichText : public QThread
{
    Q_OBJECT
public:
    Thread_loadRichText(int n, QFileInfoList f, Paper* p);
protected:
    void run();
private:
    int num;
    QFileInfoList list;
    Paper *paper;
signals:
    void hasDone(QString, Paper*);
};

#endif // THREAD_LOADRICHTEXT_H
