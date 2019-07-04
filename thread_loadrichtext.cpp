#include "thread_loadrichtext.h"

Thread_loadRichText::Thread_loadRichText(int n, QFileInfoList f, Paper* p):
    num(n), list(f), paper(p)
{

}

void Thread_loadRichText::run()
{
    qDebug() << "我在加载富文本的线程里";
    QString str = RichTextProcess::extractRichText(num);
    emit hasDone(str, paper);
    this->quit();
    this->deleteLater();
}
