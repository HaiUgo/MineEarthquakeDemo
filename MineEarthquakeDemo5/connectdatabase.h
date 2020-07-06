#ifndef CONNECTDATABASE_H
#define CONNECTDATABASE_H

#include <QSqlDatabase>
#include <QDebug>

class ConnectDataBase{
public:
    void conDataBase();  //连接数据库
    void close();
private:
    QSqlDatabase db;
};



#endif                  // CONNECTDATABASE_H
