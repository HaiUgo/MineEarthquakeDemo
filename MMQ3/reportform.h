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

public:
    explicit ReportForm(QWidget *parent = nullptr);
    ~ReportForm();
private slots:
    void on_axWidget_ImplementCommandEvent(int iCommandId);
    void queryButtonClicked();
    void clearTwinkeButtonClickled();

    void generateDOCClicked();
    void printResultClicked();
    void changeContentButtonClicked();
    void restoreCADButtonClicked();
    void moveCADButtonClicked();

    void onStartDateChanged(const QDate &date);
    void onEndDateChanged(const QDate &date);
};


#endif // REPORTFORM_H
