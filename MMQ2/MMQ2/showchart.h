#ifndef SHOWCHART_H
#define SHOWCHART_H

/***************************************************************************************
** 描述：子界面ShowChart，用来显示波形图，提供了一些功能操作
** 功能：图表初始化，切换台站，刷新P波位置，调用定位算法，更新CSV文件以及键鼠事件
** 作者：于海友
****************************************************************************************/


#include <QWidget>
#include <QDebug>
#include <QTimer>
#include <QLabel>
#include <QThread>
#include <QMessageBox>
#include <QStatusBar>
#include "connectdatabase.h"
#include "chart.h"
#include "chartview.h"
#include "readcsvdata.h"
#include "param.h"
#include "drawthread.h"
#include "locationalgorithm.h"
#include "statusbarmonitoring.h"
#include "screenconfigure.h"

QT_CHARTS_USE_NAMESPACE

extern QStatusBar *globalStatusBar;
class ReadCSVData;
class DynamicWave;

namespace Ui {
class ShowChart;
}

class ShowChart : public QWidget
{
    Q_OBJECT

public:
    explicit ShowChart(QWidget *parent = nullptr);
    ~ShowChart();

    friend class DrawThread;

    static int USERINPUT[3];                                       //保存用户选择的台站和计算方向以及用户输入的要计算的数值，
                                                                   //其中计算方向目前仅计算了z方向的激发位置，所以用户选择XY方向时不做处理

private:
    void initCharts();                                           //初始化曲线图表
    void showStackedWidgetCharts();                              //将标签和view[T]图表添加到stackWidget的GridLayout布局中
    void charViewEventFilter(QEvent *event,QChart *tempChart);   //chartView事件过滤封装函数
    bool eventFilter(QObject *obj,QEvent *event);
    void handleTheInputData();                                   //处理用户手动输入的P波到时数据
    void drawSplineWave();                                      //绘制曲线图
private slots:

    void fullChartsButtonClicked();                              //显示全部台站波形按钮
    void getLoactionData();                                      //调用定位算法
    void informationDialog();                                    //消息提示对话框
    void slotPointHoverd(const QPointF &point, bool state);      //鼠标移动到chartview某点，可以显示数据

    void on_nextPage_clicked();                                  //下一页
    void on_previousPage_clicked();                              //上一页

    void on_T1Button_clicked();                                  //T1~T9按钮
    void on_T2Button_clicked();
    void on_T3Button_clicked();
    void on_T4Button_clicked();
    void on_T5Button_clicked();
    void on_T6Button_clicked();
    void on_T7Button_clicked();
    void on_T8Button_clicked();
    void on_T9Button_clicked();

    void clearAllChartDatas();
    void getChartsYAxis();                                        //接收widget发来的信号，从而动态调整ShowChart图表的Y轴范围
    void pageSwithTo9();                                          //接收widget发来的信号，从而切换相应台站
    void txIsChecked(bool checked);                               //QRadioButton：tx,ty,tz的选中状态
    void tyIsChecked(bool checked);
    void tzIsChecked(bool checked);
    void getCurrentMotipos(int value);                            //获取当前台站的激发位置
    void repaintPWave();                                          //重新绘制P波红线
    void saveModifiedPWaveData();                                 //将输入的P波到时位置更新到CSV数据文件中
    void updatePSOandPwaveData();                                 //将P波到时位置更新到CSV数据文件中以及将定位算法得到得值更新到数据库中
private:
    Ui::ShowChart *ui;

    QSplineSeries *splineSeries;                                  //曲线序列
    QLineSeries *lineSeries;                                      //折线图数据序列
    QScatterSeries *scatterSeries;                                //散点序列，虽然存有数据，但是项目相关功能中未使用到
    Chart *chart;                                                 //图表
    QValueAxis *axisX;                                            //X坐标轴
    QValueAxis *axisY;                                            //Y坐标轴
    ChartView *view;                                              //视图

    //这里的图表相关的指针其概念和上面的相同，但是因为stackwidget组件不能添加相同对象
    //即添加相同对象后上一个对象的所属指针会被释放掉，因此T1~T9按钮和全部按钮所显示的图表
    //不可兼得，因此要再另外定义一组对象。下面定义的指针用于显示全部站台的数据，其数据和每个
    //站台的数据是相等的。
    QSplineSeries *splineSeries2;
    QLineSeries *lineSeries2;
    QScatterSeries *scatterSeries2;
    Chart *chart2;
    QValueAxis *axisX2;
    QValueAxis *axisY2;
    ChartView *view2;

    QVector<QPointF> pointBuffer;                                  //坐标点缓存

    QSharedPointer<ReadCSVData> readData;                          //定义一个读取CSV文件数据的对象

    int pageSwitchIndex;                                           //stackWidget页面索引，用于切换当前页面
    QLabel *m_valueLabel;                                          //用于显示某点数据

signals:
    void saveModifiedPWave();                                      //对话框中点击修改按钮发送信号
    void updatePSOandPwave();                                      //对话框中点击更新按钮发送信号
};



#endif // SHOWCHART_H
