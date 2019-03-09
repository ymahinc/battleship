#include "mainwindow.h"
#include <QApplication>

#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    const QStringList arguments = a.arguments();
    QString file;
    if (arguments.size() > 1 && QFile::exists(arguments.last())) {
        file = arguments.last();
    }

    MainWindow w(file);
    w.show();

    return a.exec();
}
