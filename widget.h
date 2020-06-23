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

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    QSqlDatabase db;

    void connectMysql();
    void showTable();
    int  readCSVFile();
private slots:
    void on_axWidget_ImplementCommandEvent(int iCommandId);  //控件命令

    void on_zoomIn_clicked(); //放大按钮

    void on_startButton_clicked(); //运行按钮

    void on_zoomOut_clicked();   //缩小按钮

    void on_restoreButton_clicked();  //还原按钮

    void on_dailyStatement_clicked(); //日报表按钮

    void on_monthlyStatement_clicked(); //月报表按钮

    void on_helpButton_clicked();  //帮助按钮

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
