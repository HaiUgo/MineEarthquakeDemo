#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    connectMysql();
}

Widget::~Widget()
{
    db.close();
    delete ui;
}


//连接数据库
void Widget::connectMysql()
{
    db = QSqlDatabase::addDatabase("QMYSQL");  //连接的MYSQL的数据库驱动
    db.setHostName("127.0.0.1");      //连接数据库主机名，这里需要注意（若填的为”127.0.0.1“，出现不能连接，则改为localhost)
    db.setPort(3306);                    //端口
    db.setDatabaseName("minedemo");          //数据库名
    db.setUserName("root");              //用户名
    db.setPassword("071139");            //密码
    db.open();

    //测试连接
    if(!db.open())
    {
        qDebug()<<"不能连接"<<"connect to mysql error"<<db.lastError().text();
        return ;
    }
    else
    {
         qDebug()<<"连接成功"<<"connect to mysql OK";
    }

}

//控件命令按钮
void Widget::on_axWidget_ImplementCommandEvent(int iCommandId)
{
    if(iCommandId == 1)
    {
        // 调用控件打开函数，打开.dwg文件。
        ui->axWidget->dynamicCall("OpenDwgFile(const QString&)","C:\\Users\\13696\\Desktop\\项目参考资料\\test.dwg");
    }
    if(iCommandId == 5)
    {
        ui->axWidget->dynamicCall("ZoomScale(double dScale)",0.8);
    }
    if(iCommandId == 6)
    {
        ui->axWidget->dynamicCall("ZoomScale(double dScale)",1.1);
    }
    if(iCommandId == 7)
    {
        ui->axWidget->dynamicCall("ZommAll()");
    }
}


//运行按钮
void Widget::on_startButton_clicked()
{
    // 执行控件自定义命令函数，命令的id为2，这个id用户可以自己取任意数,可以理解为是给执行的命令一个编号.
    ui->axWidget->dynamicCall("DoCommand(const qint32&)",1);
    showTable();
    readCSVFile();
}

//放大按钮
void Widget::on_zoomIn_clicked()
{
    ui->axWidget->dynamicCall("DoCommand(const qint32&)",5);
}

//日报表
void Widget::on_dailyStatement_clicked()
{

}

//月报表按钮
void Widget::on_monthlyStatement_clicked()
{

}

//帮助按钮
void Widget::on_helpButton_clicked()
{

}

//缩小按钮
void Widget::on_zoomOut_clicked()
{
    ui->axWidget->dynamicCall("DoCommand(const qint32&)",6);
}

//还原按钮
void Widget::on_restoreButton_clicked()
{
    ui->axWidget->dynamicCall("DoCommand(const qint32&)",7);
}

//显示数据表
void Widget::showTable()
{
    QSqlQueryModel * model = new QSqlQueryModel(this);
    model->setQuery("select * from basicdata");
    model->setHeaderData(0,Qt::Horizontal,tr("事件序号"));
    model->setHeaderData(1,Qt::Horizontal,tr("触发时间"));
    model->setHeaderData(2,Qt::Horizontal,tr("触发站台"));
    model->setHeaderData(3,Qt::Horizontal,tr("定位坐标"));
    model->setHeaderData(4,Qt::Horizontal,tr("能量/J"));
    model->setHeaderData(5,Qt::Horizontal,tr("震级"));

    ui->tableView_2->setModel(model);

   // qDebug()<<ui->tableView_2->currentIndex().row()<<" ";
//    调试用例
//    QSqlQuery query(db);
//    query.exec("select * from student");
//    while(query.next())
//    {
//        qDebug()<<query.value("sname").toString();
//    }

}


//在Table Widget上显示CSV文件数据
int Widget::readCSVFile()
{
    //QFileDialog *dialog = new QFileDialog(this);
   // QString fileName = dialog->getOpenFileName(this,tr("Open File"),"C:/Users/13696/Desktop/项目参考资料",tr("Excel(*.csv)"));
    QString fileName = "C:/Users/13696/Desktop/项目参考资料/partcsvdata.csv";
    if(fileName=="")
    {
        return -1;
    }

    QDir dir = QDir::current();
    QFile file(dir.filePath(fileName));
    if(!file.open(QIODevice::ReadOnly))
    {
        return -1;
    }

    QList<QStringList> data;
    QStringList row;
    QString line ;

    int columnCount = 0;
    int rowCount = 0;
    bool isTheFirstLine = true;

    QTextStream stream(&file);

    while (!stream.atEnd())
    {
        line = stream.readLine();
        if(line.isEmpty()){
            break;
        }

        for(const QString& cell: line.split(','))
        {
            row.append(cell.trimmed());
            if(isTheFirstLine)
            {
                columnCount++;
                qDebug()<<"columnCount = "<<columnCount<<"\n";
            }
        }
        rowCount++;
        isTheFirstLine = false;

        data.append(row);
    }

    ui->tableWidget->setRowCount(rowCount);
    ui->tableWidget->setColumnCount(columnCount);
    for (int countrow = 0,i=0;countrow < rowCount; countrow++,i++)
        for(int countcolumn = 0; countcolumn < columnCount; countcolumn++)
        {
            ui->tableWidget->setItem(countrow,countcolumn,new QTableWidgetItem(data.at(i)[countcolumn]));
        }

    file.close();

//调试用例
//    for(int i=0; i<data.size(); ++i)
//    {
//        QStringList s1=data.at(i);
//        QString str=s1.join(" ") + "!";
//        qDebug()<<str<<"\n";
//        qDebug()<<"i="<<i<<"\n";
//    }
    return 0;
}



