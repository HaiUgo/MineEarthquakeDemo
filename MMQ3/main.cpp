#include "widget.h"
#include "reportform.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.showMaximized();

    return a.exec();
}
