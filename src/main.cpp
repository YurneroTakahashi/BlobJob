#include "include/gamewidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    gameWidget w;
    w.show();
    return a.exec();
}
