#ifndef CONNECTDATABASE_H
#define CONNECTDATABASE_H

/**************************************************************************************
** 描述：该类对象可用来连接MySQL数据库，提供了conDataBase()接口来访问数据库
** 功能：连接数据库
** 作者：于海友
***************************************************************************************/


#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QMessageBox>
#include "databaseconfigure.h"

class ConnectDataBase{
public:
    void conDataBase();                                 //连接数据库
    void close();
    //更新数据表的某行数据
    void updateDataBase(int id,double xData,double yData,double zData,double Parrival);
public:
    static int EVENTID;                                 //表示数据库中哪个事件要被修改
private:
    QSqlDatabase db;
};



#endif                                                 // CONNECTDATABASE_H