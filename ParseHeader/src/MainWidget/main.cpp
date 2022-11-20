#include <QtWidgets/QApplication>

#include "ParseHeaderWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ParseHeaderWidget w;
    w.show();
    return a.exec();
}
