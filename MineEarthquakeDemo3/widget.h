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

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void connectMysql();
    void showTable();

    void initSplineWaveChart(); //初始化图表
    void initSplineWaveChart2();
    void initSplineWaveChart3();

    bool eventFilter(QObject *obj,QEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void charViewEventFilter(QEvent *event,QChart *tempChart); //chartView事件过滤封装函数
private slots:
    void on_axWidget_ImplementCommandEvent(int iCommandId);  //控件命令

    void on_zoomIn_clicked(); //放大按钮
    void on_startButton_clicked(); //运行按钮
    void on_zoomOut_clicked();   //缩小按钮
    void on_restoreButton_clicked();  //还原按钮
    void on_dailyStatement_clicked(); //日报表按钮
    void on_monthlyStatement_clicked(); //月报表按钮
    void on_helpButton_clicked();  //帮助按钮

    void timeOutEvent();  //定时器超时事件

    void displayButtonClick(); //显示波形图按钮
    void stopButtonClick();    //暂停按钮
    void on_fulllScreenButton_clicked(); //全屏按钮

    void switchPage();   //切换页面

    void on_T1Button_clicked();  //T1站台
    void on_t1X_clicked(); //T1站台X方向
    void on_t1Y_clicked();
    void on_t1Z_clicked();

    void on_T2Button_clicked();
    void on_T3Button_clicked();

private:
    Ui::Widget *ui;

    QSqlDatabase db;

    QVector<QPointF> pointBuffer;
    QTimer *drawTimer;  //定时器
    bool isStoppingTimer; //判断定时器是否开启

    QStringList originList;    //CSV数据列表
    QString origin;

    int originListSize;    //CSV数据长度
    int originListIndex;    //当前CSV数据索引

    QSplineSeries *splineSeries;
    QScatterSeries *scatterSeries;
    QChart *chart;
    QValueAxis *axisX;
    QValueAxis *axisY;

    QSplineSeries *splineSeries2;
    QScatterSeries *scatterSeries2;
    QChart *chart2;
    QValueAxis *axisX2;
    QValueAxis *axisY2;

    QSplineSeries *splineSeries3;
    QScatterSeries *scatterSeries3;
    QChart *chart3;
    QValueAxis *axisX3;
    QValueAxis *axisY3;

    QChartView *view;
    QChartView *view2;
    QChartView *view3;

    QVBoxLayout *chartViewLayout1;
    QWidget *chartViewWidget1;
    QVBoxLayout *chartViewLayout2;
    QWidget *chartViewWidget2;
    QVBoxLayout *chartViewLayout3;
    QWidget *chartViewWidget3;
    void drawSplineWave(int axis_x, double data); //绘制曲线图

    bool isClickingChart; //chart图表鼠标移动事件的开关
    int xOld;    //chart图表鼠标移动的X轴距离
    int yOld;    //chart图表鼠标移动的Y轴距离

    bool isFirstReadingCSV;
};
#endif // WIDGET_H
