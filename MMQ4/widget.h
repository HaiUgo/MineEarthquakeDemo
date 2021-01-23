#ifndef WIDGET_H
#define WIDGET_H

/****************************************************************************************
** 描述：主界面，包含MxDraw控件的调用，数据库连接，子界面的实例化
** 功能：矿区图显示相关，数据库行事件双击操作，子线程的实例化以及与子界面的通信操作
** 作者：于海友
*****************************************************************************************/


#include <QWidget>
#include <QLabel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QFileDialog>
#include <QThread>
#include <QStatusBar>
#include <QComboBox>
#include <QVector>
#include <QQueue>
#include <QMutex>
#include <QMutexLocker>
#include "chart.h"
#include "chartview.h"
#include "connectdatabase.h"
#include "databaseconfigure.h"
#include "reportform.h"
#include "assistant.h"
#include "miningareamap.h"
#include "miningarealocation.h"
#include "locationalgorithm.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

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

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_axWidget_ImplementCommandEvent(int iCommandId);      //axWidget控件命令

    void zoomInClicked();                                        //放大按钮
    void zoomOutClicked();                                       //缩小按钮
    void restoreButtonClicked();                                 //还原按钮
    void moveViewButtonClicked();                                //移动视区按钮
    void cancleDynBlinkClicked();                                //取消标记

    void startButtonClicked();                                   //运行按钮
    void dailyStatementClicked();                                //日报表按钮
    void helpButtonClicked();                                    //帮助按钮
    void dataBaseViewDC(const QModelIndex &);                    //dataBaseView鼠标双击事件
    void refreshCurrentDataTable();                              //刷新当前数据表

    void charViewEventFilter(QEvent *event,QChart *tempChart);   //chartView事件过滤封装函数
    bool eventFilter(QObject *obj,QEvent *event);

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

    void getCurrentMotipos(int value);                            //获取当前台站的激发位置
    void repaintPWave();                                          //重新绘制P波红线

private:
    Ui::Widget *ui;

    ConnectDataBase *db;                                         //定义一个连接数据库的对象
    QSqlQueryModel * sqlModel;                                  //sql模型
    //QSqlTableModel *sqlModel;

    double coordinates[3];                                       //矿区图X,Y,Z坐标
    QString datatable;                                     //当前数据表名

    QMap<QString,QString> miningAreaLocation;
    QMap<QString,QString> miningAreaMap;
    QString  whichRegion ;                                            //表示选择的哪一个地方，是红阳还是大同等
    QVector<qlonglong> rowCount;                                 //记录用户在数据库视图上操作了哪些行事件
    QVector<qlonglong>::iterator iter;
    QQueue<qlonglong> *blinkId ;                                 //存储实体闪烁ID

    QStatusBar *globalStatusBar;

    QSplineSeries *splineSeries;                                  //曲线序列
    QLineSeries *lineSeries;                                      //折线图数据序列
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
    Chart *chart2;
    QValueAxis *axisX2;
    QValueAxis *axisY2;
    ChartView *view2;

    int pageSwitchIndex;                                           //stackWidget页面索引，用于切换当前页面
    QLabel *m_valueLabel;                                          //用于显示某点数据

    QVector<QPointF> *pointBufferX;    //存储坐标点序列
    QVector<QPointF> *pointBufferY;    //存储坐标点序列
    QVector<QPointF> *pointBufferZ;    //存储坐标点序列
    QVector<QPointF> *pointBuffer_P;  //用于绘制P波到时

    //存储台站XYZ通道数据中的最大值用来动态调整图表的Y轴范围
    int maxValueOfZChannel;
    int maxValueOfXChannel;
    int maxValueOfYChannel;

    int userInput;

    QString filePath;

    int *motiPos;                            //每个台站的激发位置
    QString *motiPosName;

private :
    bool readCSVFileOfXYZ(QString fileName);
    void drawWave();
    void initCharts();

    void saveModifiedPWaveData();                                 //将输入的P波到时位置更新到CSV数据文件中
    void updatePSOandPwaveData();                                 //将P波到时位置更新到CSV数据文件中以及将定位算法得到得值更新到数据库中
    void showTable();
};

#endif // WIDGET_H
