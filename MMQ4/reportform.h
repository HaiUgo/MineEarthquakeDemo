#ifndef REPORTFORM_H
#define REPORTFORM_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QMessageBox>
#include <QSqlRecord>
#include <QQueue>
#include <QDate>
#include <QAxWidget>
#include <QAxObject>
#include <QFileDialog>
#include <QDesktopServices>
#include <QPrinter>
#include <QTextBlock>
#include <QTextDocument>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QTimer>
#include <QDirIterator>

#include "chart.h"
#include "chartview.h"
#include "connectdatabase.h"
#include "miningareamap.h"
#include "miningarealocation.h"
#include "databaseconfigure.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class ReportForm;
}
QT_END_NAMESPACE

class ReportForm:public QWidget{
    Q_OBJECT
private:
    Ui::ReportForm *ui;

    ConnectDataBase *db;                                         //定义一个连接数据库的对象

    QQueue<qlonglong> *blinkId ;                                 //存储实体闪烁ID
    QQueue<qlonglong> *textId;                                   //存储文字ID
    QQueue<qlonglong> *pointId;                                   //存储中点ID

    QMap<QString,QList<QVariant>> mapParameter;

    QMap<QString,QString> miningAreaMap;
    QString miningAreaValue;
    QMap<QString,QString> miningAreaLocation;

    QString startDate;
    QString endDate;
    QString datatable;


    //存储台站XYZ通道数据中的最大值用来动态调整图表的Y轴范围
    int maxValueOfZChannel;

    QSplineSeries *splineSeries;                                  //曲线序列
    QLineSeries *lineSeries;                                      //折线图数据序列
    Chart *chart;                                                 //图表
    QValueAxis *axisX;                                            //X坐标轴
    QValueAxis *axisY;                                            //Y坐标轴
    ChartView *view;                                              //视图

    QVector<QPointF> *pointBuffer;    //存储坐标点序列
    QVector<QPointF> *pointBuffer_P;  //用于绘制P波到时

    QString filePathOnTableWidget;    //获取的表格中的文件路径

    QList<QString> listOfFilePath;    //保存查询到的文件路径表列，用于自动截图读取文件路径

    void initZCharts();
    bool readCSVFileOfZ(QString fileName);
public:
    explicit ReportForm(QWidget *parent = nullptr);
    ~ReportForm();
public:
    QQueue<QString>  wenjianPath;
    QQueue<QString>  panfuhao;
private slots:
    void on_axWidget_ImplementCommandEvent(int iCommandId);
    void queryButtonClicked();
    void clearTwinkeButtonClickled();

    void generateDOCClicked();
    void generateWebDOCClicked();
    void captureByRobotClicked();
    void restoreCADButtonClicked();
    void moveCADButtonClicked();

    void onStartDateChanged(const QDate &date);
    void onEndDateChanged(const QDate &date);


    void getDoubleClickedItem(QTableWidgetItem *item);
    void getSingleClickedItem(QTableWidgetItem* item);

};


#endif // REPORTFORM_H