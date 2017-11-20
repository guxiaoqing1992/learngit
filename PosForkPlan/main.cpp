#include "mainwindow.h"

#include <QApplication>
#include <QtCore/QFile>
#include <QResource>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Fork计划");
    w.show();

    /*加载资源文件*/
    QFile styleFile(":/new/prefix1/style.qss");
    styleFile.open(QIODevice::ReadOnly);
    QString setStyleSheet(styleFile.readAll());
    a.setStyleSheet(setStyleSheet);

    return a.exec();
}
