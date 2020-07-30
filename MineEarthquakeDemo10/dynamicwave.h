#ifndef DYNAMICWAVE_H
#define DYNAMICWAVE_H

#include <QWidget>
#include <QDebug>
#include <QTimer>
#include <QLabel>
#include <QCloseEvent>
#include <QPoint>
#include <QtMath>
#include "chart.h"
#include "chartview.h"
#include "readcsvdata.h"

QT_CHARTS_USE_NAMESPACE

class ReadCSVData;

namespace Ui {
class DynamicWave;
}

class DynamicWave : public QWidget
{
    Q_OBJECT

public:
    explicit DynamicWave(QWidget *parent = 0);
    ~DynamicWave();

private:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void initDynamicCharts();                            //初始化图表

    void charViewEventFilter(QEvent *event,QChart *tempChart);   //chartView事件过滤封装函数
    bool eventFilter(QObject *obj,QEvent *event);

private:
    Ui::DynamicWave *ui;

    QSplineSeries *splineSeries;      //曲线序列
    QLineSeries *lineSeries;          //折线图数据序列
    Chart *chart;                     //图表
    QValueAxis *axisX;                //X坐标轴
    QValueAxis *axisY;                //Y坐标轴
    ChartView *view;                  //视图

    int axis_x_counts;                //横坐标点计数
    QTimer *drawTimer;                //定时器
    bool isStoppingTimer;             //判断定时器是否开启

    ReadCSVData *readData;            //定义一个读取CSV文件数据的对象

    int userInput[2];                 //保存用户选择的台站以及输入的P波激发位置

    QLabel *m_valueLabel;             //用于显示某点数据

    QVector<QPointF> *pointBufferTemp;   //用于提前添加坐标点（listIndex,0），然后才可以出队列数据替换现有的数据
    int listIndex;
    int queueSize;                       //表示台站某一方向的队列数据的长度

private slots:
    void drawDynSplineWave();            //绘制动态波形
    void stopDynWave();                  //显示波形
    void showDynWave();                  //停止显示
    void slotPointHoverd(const QPointF &point, bool state);   //鼠标移动到chartview某点，可以显示数据
    void receiveCSVFilePath2(QString path); //接收widget发来的信号，从而获取相应CSV文件路径

};


#endif // DYNAMICWAVE_H
