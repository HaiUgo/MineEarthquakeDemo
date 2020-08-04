#ifndef DRAWTHREAD_H
#define DRAWTHREAD_H

#include <QObject>
#include <QString>
#include "showchart.h"
#include "readcsvdata.h"
class ShowChart;
class ReadCSVData;
extern ShowChart *showChart;             //子界面，即chartview界面

class DrawThread:public QObject
{
    Q_OBJECT
public:
    explicit DrawThread(QObject *parent = nullptr);
    ~DrawThread();
public slots:
    void doDrawSplineWork();
private:
    QSharedPointer<ReadCSVData> readData;            //定义一个读取CSV文件数据的对象
    QMutex mutex;
};

#endif // DRAWTHREAD_H
