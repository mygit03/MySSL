#include "myssl.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MySSl w;
    w.show();

    return a.exec();
}
