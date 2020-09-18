#ifndef READCSVDATA_H
#define READCSVDATA_H

/****************************************************************************************
** 描述：读取CSV文件
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
#include <QThread>
#include "param.h"
#include "drawthread.h"
#include "statusbarmonitoring.h"


class ReadCSVData: public QThread
{
    Q_OBJECT
public:
    ReadCSVData(QObject *parent = nullptr);
    ~ReadCSVData();
public:
    static QString FILEPATH;                 //CSV文件的文件路径
    static QString PANFU;                    //CSV文件中的盘符名
    static int TEMPMOTIPOS[10];              //存储具体站台名称及其激发位置，比如TEMPMOTIPOS[1]表示台站T1的激发位置
    static int TEMPSTATION[10];              //按照CSV文件中站台顺序存储站台名称
    static int SENNUM ;                      //记录传感器的数量
    static int COUNT ;                       //记录事件的个数，在CSV文件中表现为行数

    const static int STARTTIME = 3;          //开始的截取时间，激发位置前面5s
    const static int ENDTIME = 15;           //结束的截取时间，激发位置后面10s
    const static int FREQUENCY=4800;         //频率

    static QVector<QPointF> *POINTBUFFER;    //存储坐标点序列
    static QVector<QPointF> *POINTBUFFER_P;  //用于绘制P波到时
public:
    QString **senChannelZ ;                  //存储每个事件后三个通道中Z轴的数据
    QString **senChannelY ;                  //存储每个事件后三个通道中Y轴的数据
    QString **senChannelX ;                  //存储每个事件后三个通道中X轴的数据
    QString **senChannelNum ;                //存储每个事件触发台站名称
    //QString *date;                           //存储每个事件的日期
    int *motiPos;                            //每个台站的激发位置

    void readCSVFile(QString fileName);      //读取CSV文件中的数据
    void parseCSVFileName(QString filePath); //解析CSV文件路径
    void locateCSVData();                    //定位事件站台在CSV文件哪一列，从而定位到该站台X，Y，Z轴以及激发位置在哪一列
    void paddingPointBuffer(QVector<QPointF> *pointBufferX,QVector<QPointF> *pointBufferY,QVector<QPointF> *pointBufferZ,int index);
    void stop();
protected:
    void run();
private:
    volatile bool stopped;
};


#endif // READCSVDATA_H
