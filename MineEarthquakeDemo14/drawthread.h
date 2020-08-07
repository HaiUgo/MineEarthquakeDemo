#ifndef DRAWTHREAD_H
#define DRAWTHREAD_H

/*************************************************************************************
** 描述：开启子线程。
** 功能：将波形图绘制操作转移到子线程来提高效率
** 作者：于海友
**************************************************************************************/


#include <QObject>
#include <QString>
#include "showchart.h"
#include "readcsvdata.h"
class ShowChart;
class ReadCSVData;
extern ShowChart *showChart;                         //子界面

class DrawThread:public QObject
{
    Q_OBJECT
public:
    explicit DrawThread(QObject *parent = nullptr);
    ~DrawThread();
public slots:
    void doDrawSplineWork();                         //绘制波形图
private:
    QSharedPointer<ReadCSVData> readData;            //定义一个读取CSV文件数据的对象
};

#endif // DRAWTHREAD_H
