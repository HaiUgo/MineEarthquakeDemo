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

class ConnectDataBase{
public:
    void conDataBase();  //连接数据库
    void close();
    static void updateDataBase(QString table,int id,double xData,double yData,double zData,double Parrival);
private:
    QSqlDatabase db;
};



#endif                  // CONNECTDATABASE_H
