#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //设置全局字体样式
    QFont font;
    font.setPointSize(11);
    font.setFamily("宋体");
    a.setFont(font);
    MainWindow w;
    w.show();

    return a.exec();
}
