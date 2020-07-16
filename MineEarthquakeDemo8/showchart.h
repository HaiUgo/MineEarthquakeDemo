#ifndef SHOWCHART_H
#define SHOWCHART_H


#include <QWidget>
#include <QDebug>
#include <QTimer>

#include "chart.h"
#include "chartview.h"
#include "readcsvdata.h"

QT_CHARTS_USE_NAMESPACE

//T为1-9台站每个曲线图表的XYZ三轴方向，也用作计数
enum T{
    T1X,T1Y,T1Z,
    T2X,T2Y,T2Z,
    T3X,T3Y,T3Z,
    T4X,T4Y,T4Z,
    T5X,T5Y,T5Z,
    T6X,T6Y,T6Z,
    T7X,T7Y,T7Z,
    T8X,T8Y,T8Z,
    T9X,T9Y,T9Z
};


namespace Ui {
class ShowChart;
}

class ShowChart : public QWidget
{
    Q_OBJECT

public:
    explicit ShowChart(QWidget *parent = 0);
    ~ShowChart();


private:
    void initCharts();                              //初始化曲线图表
    void showStackedWidgetCharts();                 //将标签和view[T]图表添加到stackWidget的GridLayout布局中
    void drawSplineWave();                          //绘制曲线图
    //void charViewEventFilter(QEvent *event,QChart *tempChart);   //chartView事件过滤封装函数
    //bool eventFilter(QObject *obj,QEvent *event);
    //void mouseMoveEvent(QMouseEvent *event);
   // void mouseReleaseEvent(QMouseEvent *event);
private slots:
    void displayButtonClick();        //显示波形图按钮
    void stopButtonClick();           //暂停按钮
    void fullChartsButtonClicked();   //显示全部波形按钮

    void on_nextPage_clicked();       //下一页
    void on_previousPage_clicked();   //上一页

    void on_T1Button_clicked();       //T1~T9按钮
    void on_T2Button_clicked();
    void on_T3Button_clicked();
    void on_T4Button_clicked();
    void on_T5Button_clicked();
    void on_T6Button_clicked();
    void on_T7Button_clicked();
    void on_T8Button_clicked();
    void on_T9Button_clicked();

private:
    Ui::ShowChart *ui;

    QSplineSeries *splineSeries;      //曲线序列
    QLineSeries *lineSeries;          //折线图数据序列
    QScatterSeries *scatterSeries;    //点序列
    Chart *chart;                     //图表
    QValueAxis *axisX;                //X坐标轴
    QValueAxis *axisY;                //Y坐标轴
    ChartView *view;                  //视图

    //这里的图表相关的指针其概念和上面的相同，但是因为stackwidget组件不能添加相同对象
    //即添加相同对象后上一个对象的所属指针会被释放掉，因此T1~T9按钮和全部按钮所显示的图表
    //不可兼得，因此要再另外定义一组对象。下面定义的指针用于显示全部站台的数据，其数据和每个
    //站台的数据是相等的。
    QSplineSeries *splineSeries2;      //曲线序列
    QLineSeries *lineSeries2;          //折线序列
    QScatterSeries *scatterSeries2;    //点序列
    Chart *chart2;                     //图表
    QValueAxis *axisX2;                //X坐标轴
    QValueAxis *axisY2;                //Y坐标轴
    ChartView *view2;                  //视图

    QVector<QPointF> pointBuffer;     //坐标点缓存
    QTimer *drawTimer;                //定时器
    bool isStoppingTimer;             //判断定时器是否开启

    ReadCSVData *readData;            //定义一个读取CSV文件数据的对象

    int pageSwitchIndex;              //stackWidget页面索引，用于切换当前页面

    QString filePath;                 //CSV文件路径
    QStringList originList;           //CSV数据列表
    QString origin;
    int originListSize;               //CSV数据长度
    int originListIndex;              //当前CSV数据索引

    bool isClickingChart;                           //chart图表鼠标移动事件的开关
    int xOld;                                       //chart图表鼠标移动的X轴距离
    int yOld;                                       //chart图表鼠标移动的Y轴距离
};



#endif // SHOWCHART_H