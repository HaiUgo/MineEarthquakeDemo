#include "connectdatabase.h"


int ConnectDataBase::EVENTID = 0;

QString ConnectDataBase::SQLTABLE = "";

int ConnectDataBase::WHICHREGION = 0;

//连接数据库
void ConnectDataBase::conDataBase()
{
    db = QSqlDatabase::addDatabase("QMYSQL");  //连接的MYSQL的数据库驱动
    db.setHostName("localhost");               //连接数据库主机名，这里需要注意（若填的为”127.0.0.1“，出现不能连接，则改为localhost)
    db.setPort(3306);                          //端口
    db.setDatabaseName("minedemo");            //数据库名
    db.setUserName("root");                    //用户名
    db.setPassword("071139");                  //密码
    db.open();

    if(!db.open()){                            //打开
        qDebug()<<"the connection to database is failed";
        return ;
    }
    else{
         qDebug()<<"the connection to database is successful";
    }
}

//释放连接
void ConnectDataBase::close()
{
    db.close();
    qDebug()<<"ConnectDataBase::close()";
}

void ConnectDataBase::updateDataBase(QString table,int id,double xData,double yData,double zData,double Parrival)
{
    QSqlQuery query;
    if(table == "mine_quack_5_results"){
        query.prepare("update mine_quack_5_results  set xData=?,yData=?,zData=?,Parrival=? where id =?");
    }else if(table == "mine_quack_3_results"){
        query.prepare("update mine_quack_3_results  set xData=?,yData=?,zData=?,Parrival=? where id =?");
    }else {
        qDebug()<<"database table not found!";
        return;
    }
    //query.addBindValue(table);
    query.addBindValue(xData);
    query.addBindValue(yData);
    query.addBindValue(zData);
    query.addBindValue(Parrival);
    query.addBindValue(id);
    if(query.exec())qDebug()<<"update "<<table<<" at "<<id<<"successed";
}
