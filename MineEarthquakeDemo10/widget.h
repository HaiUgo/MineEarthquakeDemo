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
#include <QMap>
#include <QThread>
#include "showchart.h"
#include "connectdatabase.h"
#include "mxdrawxlib.h"

using namespace MxDrawXLib;


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT
    QThread workThread;
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void showTable();                               //在UI上显示数据库列表

private slots:
    void on_axWidget_ImplementCommandEvent(int iCommandId);      //axWidget控件命令

    void zoomInClicked();             //放大按钮
    void zoomOutClicked();            //缩小按钮
    void restoreButtonClicked();      //还原按钮
    void moveViewButtonClicked();     //移动视区按钮
    void cancleDynBlinkClicked();     //取消标记

    void startButtonClicked();        //运行按钮
    void dailyStatementClicked();     //日报表按钮
    void monthlyStatementClicked();   //月报表按钮
    void helpButtonClicked();         //帮助按钮
    void dynamicWaveButtonClick();    //显示动态波形图按钮
    void dataBaseViewDC(const QModelIndex &);   //dataBaseView鼠标双击事件

private:
    Ui::Widget *ui;

    ShowChart *showChart;             //子界面，即chartview界面


    ConnectDataBase *db;              //定义一个连接数据库的对象

    QMap<int,qlonglong> entityId;

    DynamicWave *dw ;                 //子界面，即动态波形界面

signals:
    void pageSwitch9();
    void sendSelectedCSVFile(QString filePath);
};
#endif // WIDGET_H
