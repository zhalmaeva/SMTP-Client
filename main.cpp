#include "mainwindow.h"
#include <QApplication>
#include "MyClient.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyClient     client;
    client.show();
    return a.exec();
}
