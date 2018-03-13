#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.show();
    QMetaObject::invokeMethod(&w, "on_connectButton_clicked");

    return a.exec();
}
