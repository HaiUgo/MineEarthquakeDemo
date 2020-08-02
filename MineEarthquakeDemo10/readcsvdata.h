#ifndef READCSVDATA_H
#define READCSVDATA_H

#include <QString>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QPointF>
#include <QQueue>

#include "dynamicwave.h"
#include "showchart.h"
#include "drawthread.h"
class ReadCSVData;

//想调用QObject中的槽函数deleteLater()自动删除实例化的ReadCSVData对象，所以继承了QObject类
//但是感觉对象仍然没有释放掉，内存消耗仍然很大，其中connect的关联在子线程drawthread的doDrawSplineWork函数中
//connect(readData,SIGNAL(destroyed()),readData,SLOT(deleteLater()));
//总的来说是设计缺陷
class ReadCSVData:public QObject{
    Q_OBJECT
public:
    ReadCSVData(QObject *parent = nullptr);
    ~ReadCSVData();

    friend class DynamicWave;
    friend class ShowChart;
    friend class DrawThread;
    static int tempMotiPos[10][1];           //存储具体站台名称及其激发位置，站台标号为1-9对应tempMotiPos[1][0]~[9][0]

private:

    int senNum = 0;                          //记录传感器的数量

    QString **senChannelZ ;                  //存储每个事件后三个通道中Z轴的数据
    QString **senChannelY ;                  //存储每个事件后三个通道中Y轴的数据
    QString **senChannelX ;                  //存储每个事件后三个通道中X轴的数据

    QString **senChannelNum ;                //存储每个事件触发台站名称

    QString *date;                           //存储每个事件的日期

    QVector<QPointF> *pointBuffer;           //存储坐标点序列
    QVector<QPointF> *pointBuffer_P;         //用于绘制P波到时

    QQueue<double> *dynPointBuffer;          //存储坐标点序列，用于绘制动态波形

    int count=0;                             //记录事件的个数，在CSV文件中表现为行数
    int *motiPos;                            //每个传感器的激发位置

    const int startTime = 3;                 //开始的截取时间，激发位置前面5s
    const int endTime = 15;                  //结束的截取时间，激发位置后面10s
    const int FREQUENCY=4800;

    void readCSVFile(QString fileName);      //读取CSV文件中的数据
    void parseCSVFileName(QString filePath); //解析CSV文件路径

    void locateCSVData();                    //定位事件站台在CSV文件哪一列，从而定位到该站台X，Y，Z轴以及激发位置在哪一列
    //存储X，Y，Z轴的坐标点
    void paddingPointBuffer(QVector<QPointF> *pointBufferX,QVector<QPointF> *pointBufferY,QVector<QPointF> *pointBufferZ,int index);
    void paddingDynPointBuffer(QQueue<double> *pointBufferX,QQueue<double> *pointBufferY,QQueue<double> *pointBufferZ,int index);

};


#endif // READCSVDATA_H
