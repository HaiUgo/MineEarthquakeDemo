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

#include "databaseconfigure.h"
#include "connectdatabase.h"


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

    QString startDate;
    QString endDate;

    const QString R = "大棚";
    const QString S = "蒿子屯";
    const QString T = "杨甸子";
    const QString U = "树碑子";
    const QString V = "南风井";
    const QString W = "北青堆子";
    const QString X = "矿上车队";
    const QString Y = "火药库";
    const QString Z = "工业广场";

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
    void generatePDFClicked();
    void generateWebDOCClicked();
    void printResultClicked();
    void snippingWaveButtonClicked();
    void restoreCADButtonClicked();
    void moveCADButtonClicked();

    void onStartDateChanged(const QDate &date);
    void onEndDateChanged(const QDate &date);
};


#endif // REPORTFORM_H
