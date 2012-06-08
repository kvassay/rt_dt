#include <QtGui/QApplication>
#include "GUI/Windows/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icons/calculator"));
    MainWindow w;
    w.show();

    return a.exec();
}
