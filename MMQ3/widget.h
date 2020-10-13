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
#include "showchart.h"
#include "connectdatabase.h"
#include "databaseconfigure.h"
#include "screenconfigure.h"
#include "reportform.h"
#include "assistant.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void showTable();                                            //在UI上显示数据库列表
private slots:
    void on_axWidget_ImplementCommandEvent(int iCommandId);      //axWidget控件命令

    void zoomInClicked();                                        //放大按钮
    void zoomOutClicked();                                       //缩小按钮
    void restoreButtonClicked();                                 //还原按钮
    void moveViewButtonClicked();                                //移动视区按钮
    void cancleDynBlinkClicked();                                //取消标记

    void startButtonClicked();                                   //运行按钮
    void dailyStatementClicked();                                //日报表按钮
    //void monthlyStatementClicked();                              //月报表按钮
    void helpButtonClicked();                                    //帮助按钮
    void dataBaseViewDC(const QModelIndex &);                    //dataBaseView鼠标双击事件
    void reSelectDataBaseSource(QString value);                  //重新选择数据表
    void refreshCurrentDataTable();                              //刷新当前数据表
    void selectWhichRegion(int index);                           //选择哪一个区域，比如红阳，大同，平顶山等
    void on_currentIncidentButton_clicked();

    void on_cleanAllChartDatas_clicked();

private:
    Ui::Widget *ui;

    ConnectDataBase *db;                                         //定义一个连接数据库的对象

    QSqlQueryModel * sqlModel;                                  //sql模型
    //QSqlTableModel *sqlModel;

    double coordinates[3];                                       //矿区图X,Y,Z坐标
    QString currentDataBase;                                     //当前数据表名

    int whichRegion ;                                            //表示选择的哪一个地方，是红阳还是大同等
    QMutex mutex;
    QVector<qlonglong> rowCount;                                 //记录用户在数据库视图上操作了哪些行事件
    QVector<qlonglong>::iterator iter;
    QQueue<qlonglong> *blinkId ;                                 //存储实体闪烁ID
signals:
    void pageSwitch9();
    void sendSelectedCSVFile();
    void finishedReadDataThread();
    void cleanAllChartDatas();
    void sendChartsYAxis();
};

#endif // WIDGET_H
