#include "mainwindow.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/nadir.ico"));
    a.setApplicationName("Nadir");
    a.setApplicationVersion("1.0.0");
    a.setOrganizationName("Zyncrate Studio");

    MainWindow w;
    w.show();

    return a.exec();
}
