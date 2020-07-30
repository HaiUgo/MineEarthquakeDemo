#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.showMaximized();

//    ShowChart showChart;
//    showChart.show();
    return a.exec();
}
