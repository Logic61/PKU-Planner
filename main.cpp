#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Course Helper");
    a.setApplicationDisplayName("Course Helper");
    MainWindow w;
    w.setWindowTitle("Course Helper");
    w.show();
    return a.exec();
}