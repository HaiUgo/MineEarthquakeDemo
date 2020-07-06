#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSqlQueryModel>
#include <QFileDialog>

#include <QtCharts>
#include <QList>
#include <QSplineSeries>
#include <QScatterSeries>
#include <QChart>
#include <QChartView>

#include "readcsvdata.h"
#include "connectdatabase.h"

//T为1-9台站每个曲线图表的XYZ三轴方向
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

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void showTable();                               //在UI上显示数据库列表
    void initCharts();                              //初始化曲线图表
    void showStackedWidgetCharts();                 //将标签和view[T]图表添加到stackWidget的GridLayout布局中
    void drawSplineWave();   //绘制曲线图
    bool eventFilter(QObject *obj,QEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void charViewEventFilter(QEvent *event,QChart *tempChart);   //chartView事件过滤封装函数
private slots:
    void on_axWidget_ImplementCommandEvent(int iCommandId);      //axWidget控件命令

    void zoomInClicked();             //放大按钮
    void zoomOutClicked();            //缩小按钮
    void restoreButtonClicked();      //还原按钮
    void moveViewButtonClicked();     //移动视区按钮

    void startButtonClicked();        //运行按钮
    void dailyStatementClicked();     //日报表按钮
    void monthlyStatementClicked();   //月报表按钮
    void helpButtonClicked();         //帮助按钮

    void timeOutEvent();              //定时器超时事件

    void displayButtonClick();        //显示波形图按钮
    void stopButtonClick();           //暂停按钮
    void fullChartsButtonClicked();  //显示全部波形按钮

    void on_nextPage_clicked();       //下一页
    void on_previousPage_clicked();   //上一页

private:
    Ui::Widget *ui;

    ConnectDataBase *db;              //定义一个连接数据库的对象

    ReadCSVData *readData;            //定义一个读取CSV文件数据的对象

    QString filePath;                 //CSV文件路径

    QVector<QPointF> pointBuffer;     //坐标点缓存
    QTimer *drawTimer;                //定时器
    bool isStoppingTimer;             //判断定时器是否开启

    QStringList originList;           //CSV数据列表
    QString origin;
    int originListSize;               //CSV数据长度
    int originListIndex;              //当前CSV数据索引

    QSplineSeries *splineSeries;      //曲线序列
    QScatterSeries *scatterSeries;    //点序列
    QChart *chart;                    //图表
    QValueAxis *axisX;                //X坐标轴
    QValueAxis *axisY;                //Y坐标轴
    QChartView *view;                 //视图

    int pageSwitchIndex;                            //stackWidget页面索引，用于切换当前页面

    bool isClickingChart;                           //chart图表鼠标移动事件的开关
    int xOld;                                       //chart图表鼠标移动的X轴距离
    int yOld;                                       //chart图表鼠标移动的Y轴距离

};
#endif // WIDGET_H
