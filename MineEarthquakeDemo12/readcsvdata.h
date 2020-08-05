#ifndef READCSVDATA_H
#define READCSVDATA_H

/****************************************************************************************
** 描述：单例模式，全局只有一个ReadCSVData对象
** 功能：工具类，用来读取并解析CSV文件
** 作者：于海友
*****************************************************************************************/


#include <QString>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QPointF>
#include <QQueue>
#include <QMutexLocker>
#include <QMutex>
#include "param.h"

class ReadCSVData{

private:
    ReadCSVData();
    ReadCSVData(const ReadCSVData&){}
public:
    ~ReadCSVData();
public:
    static QSharedPointer<ReadCSVData>& getInstance();       //返回m_instance

    static QString FILEPATH;                 //CSV文件的文件路径

    static int tempMotiPos[10][1];           //存储具体站台名称及其激发位置，站台标号为1-9对应tempMotiPos[1][0]~[9][0]
    static int senNum ;                      //记录传感器的数量
    static int count ;                       //记录事件的个数，在CSV文件中表现为行数

    const static int startTime = 3;          //开始的截取时间，激发位置前面5s
    const static int endTime = 15;           //结束的截取时间，激发位置后面10s
    const static int FREQUENCY=4800;         //频率

public:
    QString **senChannelZ ;                  //存储每个事件后三个通道中Z轴的数据
    QString **senChannelY ;                  //存储每个事件后三个通道中Y轴的数据
    QString **senChannelX ;                  //存储每个事件后三个通道中X轴的数据
    QString **senChannelNum ;                //存储每个事件触发台站名称
    QString *date;                           //存储每个事件的日期
    int *motiPos;                            //每个台站的激发位置

    QVector<QPointF> *pointBuffer;           //存储坐标点序列
    QVector<QPointF> *pointBuffer_P;         //用于绘制P波到时

    void readCSVFile(QString fileName);      //读取CSV文件中的数据
    void parseCSVFileName(QString filePath); //解析CSV文件路径
    void locateCSVData();                    //定位事件站台在CSV文件哪一列，从而定位到该站台X，Y，Z轴以及激发位置在哪一列
    void paddingPointBuffer(QVector<QPointF> *pointBufferX,QVector<QPointF> *pointBufferY,QVector<QPointF> *pointBufferZ,int index);
private:
    static QSharedPointer<ReadCSVData> m_instance;            //使用共享指针指向单例对象
    static QMutex m_Mutex;                   //互斥锁
};


#endif // READCSVDATA_H
