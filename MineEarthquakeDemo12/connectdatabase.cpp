#include "connectdatabase.h"

//连接数据库
void ConnectDataBase::conDataBase()
{
    db = QSqlDatabase::addDatabase("QMYSQL");  //连接的MYSQL的数据库驱动
    db.setHostName("127.0.0.1");      //连接数据库主机名，这里需要注意（若填的为”127.0.0.1“，出现不能连接，则改为localhost)
    db.setPort(3306);                    //端口
    db.setDatabaseName("minedemo");          //数据库名
    db.setUserName("root");              //用户名
    db.setPassword("071139");            //密码
    db.open();
    //测试连接
    if(!db.open()){
        qDebug()<<"the connection to database is failed"<<'\n';
        return ;
    }
    else{
         qDebug()<<"the connection to database is successful"<<'\n';
    }
}

//释放连接
void ConnectDataBase::close()
{
    db.close();
}
