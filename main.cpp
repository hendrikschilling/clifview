#include "clifview.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ClifView w;
    w.show();

    return a.exec();
}
