#include "widget.h"
#include "ui_widget.h"

//说明：不应该将ShowChart定义为一个全局对象，但是考虑到子线程无法实例化界面对象，而且需要通过子线程调用
//ShowChart中的绘制波形图的方法来提高效率（虽然加了子线程，但是效率和之前相比并没有明显提高）
ShowChart *showChart;

//定义一个全局的状态栏，这样在子界面中也可以使用
QStatusBar *globalStatusBar;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->refreshCurrentDataTable->setEnabled(false);

    ScreenConfigure *sc = new ScreenConfigure;
    sc->getScreenConfigure();
    ui->scrollArea->setFixedSize(ScreenConfigure::width,ScreenConfigure::height);
    ui->scrollAreaWidgetContents->setFixedSize(ScreenConfigure::width,ScreenConfigure::height);

    if(DataBaseConfigure::getDataBaseConfigure()){
        db = new ConnectDataBase;
        db->conDataBase();
    }

    whichRegion = 0;

    blinkId = new QQueue<qlonglong>();

    showChart = new ShowChart(this);
    showChart->setFixedSize(ScreenConfigure::mwidth,ScreenConfigure::mheight);
    ui->scrollArea->setWidget(showChart);

    globalStatusBar = new QStatusBar(this);
    layout()->addWidget(globalStatusBar);
    globalStatusBar->showMessage(tr("欢迎来到矿山高频微震地面台网监控系统！"));

    StatusbarMonitoring *statusbarMonitoring = new StatusbarMonitoring();
    connect(statusbarMonitoring, &StatusbarMonitoring::finished, statusbarMonitoring, &QObject::deleteLater);
    statusbarMonitoring->start();

    sqlModel = new QSqlQueryModel(this);
    //sqlModel->setEditStrategy(QsqlModel::OnFieldChange);
    currentDataBase = "mine_quake_results";

    connect(ui->startButton,SIGNAL(clicked()),this,SLOT(startButtonClicked()));
    connect(ui->dailyStatement,SIGNAL(clicked()),this,SLOT(dailyStatementClicked()));
    connect(ui->monthlyStatement,SIGNAL(clicked()),this,SLOT(monthlyStatementClicked()));
    connect(ui->helpButton,SIGNAL(clicked()),this,SLOT(helpButtonClicked()));

    connect(ui->zoomIn,SIGNAL(clicked()),this,SLOT(zoomInClicked()));
    connect(ui->zoomOut,SIGNAL(clicked()),this,SLOT(zoomOutClicked()));
    connect(ui->restoreButton,SIGNAL(clicked()),this,SLOT(restoreButtonClicked()));
    connect(ui->moveViewButton,SIGNAL(clicked()),SLOT(moveViewButtonClicked()));
    connect(ui->cancelDynBlink,SIGNAL(clicked()),SLOT(cancleDynBlinkClicked()));
    connect(ui->whichRegion,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&Widget::selectWhichRegion);
    connect(ui->databaseSoureComboBox,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&Widget::reSelectDataBaseSource);
    connect(ui->refreshCurrentDataTable,SIGNAL(clicked()),SLOT(refreshCurrentDataTable()));
    connect(ui->dataBaseView,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(dataBaseViewDC(const QModelIndex &)));
    connect(this,SIGNAL(pageSwitch9()),showChart,SLOT(pageSwithTo9()));
    connect(this,SIGNAL(cleanAllChartDatas()),showChart,SLOT(clearAllChartDatas()));
    //connect(this,SIGNAL(sendChartsYAxis()),showChart,SLOT(getChartsYAxis()));

    qDebug()<<"Widget ThreadId:"<<QThread::currentThreadId();
}

Widget::~Widget()
{
    //QT对象树机制
    //释放内存空间
    delete[] ReadCSVData::POINTBUFFER;
    delete[] ReadCSVData::POINTBUFFER_P;
    //delete readData;
    delete blinkId;
    delete ui;

    qDebug()<<"Widget::~Widget()";
}

//选择哪一个区域
void Widget::selectWhichRegion(int index)
{
    ConnectDataBase::WHICHREGION = index;
    whichRegion = index;
    ui->axWidget->dynamicCall("DoCommand(const qint32&)",1);
}

//刷新当前数据表
void Widget::refreshCurrentDataTable()
{
    //sqlModel->setTable(value);

    ConnectDataBase *dataBaseConnect = new ConnectDataBase;
    dataBaseConnect->conDataBase();

    if(currentDataBase == "mine_quake_results"){
        sqlModel->setQuery("select * from "+currentDataBase);
        sqlModel->setHeaderData(0,Qt::Horizontal,tr("事件序号"));
        sqlModel->setHeaderData(1,Qt::Horizontal,tr("类型"));
        sqlModel->setHeaderData(2,Qt::Horizontal,tr("X"));
        sqlModel->setHeaderData(3,Qt::Horizontal,tr("Y"));
        sqlModel->setHeaderData(4,Qt::Horizontal,tr("Z"));
        sqlModel->setHeaderData(5,Qt::Horizontal,tr("触发时间"));
        sqlModel->setHeaderData(6,Qt::Horizontal,tr("震级"));
        sqlModel->setHeaderData(7,Qt::Horizontal,tr("到时"));
        sqlModel->setHeaderData(8,Qt::Horizontal,tr("触发站台"));
        sqlModel->setHeaderData(9,Qt::Horizontal,tr("持续时间震级"));
        sqlModel->setHeaderData(10,Qt::Horizontal,tr("能量/J"));
        sqlModel->setHeaderData(11,Qt::Horizontal,tr("路径"));
        sqlModel->setHeaderData(12,Qt::Horizontal,tr("矩张量"));
        sqlModel->setHeaderData(13,Qt::Horizontal,tr("B值"));
        QString latestDate;
        QSqlQuery query;
        query.exec( "select max(quackTime) from mine_quake_results;");            // 执行查询操作
        while(query.next()){
            latestDate = query.value(0).toString();
            qDebug()<<"date="<<latestDate;
        }
        ui->lastestIncidentLabel->setText("最新事件："+latestDate);
    }

    dataBaseConnect->close();
    delete dataBaseConnect;
}

//重新选择数据表
void Widget::reSelectDataBaseSource(QString value)
{
    currentDataBase = value;
    refreshCurrentDataTable();
}

//控件命令按钮
void Widget::on_axWidget_ImplementCommandEvent(int iCommandId)
{
    if(iCommandId == 1){
        QColor c(255,255,255);
        ui->axWidget->dynamicCall("SetViewColor(QColor)", c);

        QString path = QDir::currentPath();
        //qDebug()<<"当前路径"<<path;
        QString tempPath;
        // 调用控件打开dwg文件命令，使用相对路径
        if(0 == whichRegion){
            //QString path = QApplication::applicationDirPath();    //是获取的执行文件exe所在的路径
            tempPath=path + "/src/dwg/红阳三矿20200713lh.dwg";
            ui->axWidget->dynamicCall("OpenDwgFile(const QString&)",tempPath);
            globalStatusBar->showMessage("打开矿区图："+tempPath);
            tempPath.clear();
        }
        if(1 == whichRegion){
            tempPath=path + "/src/dwg/大同.dwg";
            ui->axWidget->dynamicCall("OpenDwgFile(const QString&)",tempPath);
            globalStatusBar->showMessage("打开矿区图："+tempPath);
            tempPath.clear();
        }
        if(2 == whichRegion){
            tempPath=path + "/src/dwg/平顶山.dwg";
            ui->axWidget->dynamicCall("OpenDwgFile(const QString&)",tempPath);
            globalStatusBar->showMessage("打开矿区图："+tempPath);
            tempPath.clear();
        }
        if(3 == whichRegion){
            tempPath=path + "/src/dwg/双鸭山.dwg";
            ui->axWidget->dynamicCall("OpenDwgFile(const QString&)",tempPath);
            globalStatusBar->showMessage("打开矿区图："+tempPath);
            tempPath.clear();
        }
    }
    if(iCommandId == 5){
        // 调用控件缩放命令。
        ui->axWidget->dynamicCall("ZoomScale(double dScale)",0.8);
    }
    if(iCommandId == 6){
        ui->axWidget->dynamicCall("ZoomScale(double dScale)",1.1);
    }
    if(iCommandId == 7){
        // 调用控件视区还原命令
        ui->axWidget->dynamicCall("ZoomAll()");
    }
    if(iCommandId == 8){
        // 调用控件移动视区命令
        ui->axWidget->dynamicCall("SendStringToExecute(P)");
    }
    if(iCommandId == 9){
        while(!blinkId->empty()){
            qlonglong temp = blinkId->dequeue();
            qDebug()<<"blikeId->size():"<<blinkId->size()<<"temp:"<<temp;
            ui->axWidget->dynamicCall("StopTwinkeEnt(qlonglong)",temp);
            ui->axWidget->dynamicCall("Clear(qlonglong)",temp);
        }
    }
}

//运行按钮
void Widget::startButtonClicked()
{ 
    ui->startButton->setEnabled(false);                      //运行按钮点击一次后就禁用
    ui->axWidget->dynamicCall("DoCommand(const qint32&)",1); //执行控件自定义命令函数，命令的id为2，该值为命令编号，可任取.
    showTable();
    ui->refreshCurrentDataTable->setEnabled(true);
}
//日报表
void Widget::dailyStatementClicked()
{

}
//月报表按钮
void Widget::monthlyStatementClicked()
{

}
//帮助按钮
void Widget::helpButtonClicked()
{

}

//放大按钮
void Widget::zoomInClicked()
{
    ui->axWidget->dynamicCall("DoCommand(const qint32&)",5);
}
//缩小按钮
void Widget::zoomOutClicked()
{
    ui->axWidget->dynamicCall("DoCommand(const qint32&)",6);
}
//移动视区按钮
void Widget::moveViewButtonClicked()
{
    ui->axWidget->dynamicCall("DoCommand(const qint32&)",8);
}
//还原按钮
void Widget::restoreButtonClicked()
{
    ui->axWidget->dynamicCall("DoCommand(const qint32&)",7);
}
//取消标记
void Widget::cancleDynBlinkClicked()
{
    ui->axWidget->dynamicCall("DoCommand(const qint32&)",9);
}

//在主界面上显示数据库数据
void Widget::showTable()
{
    //sqlModel->setTable("mine_quack_5_results");
    //sqlModel->select();
    sqlModel->setQuery("select * from mine_quake_results");
    sqlModel->setHeaderData(0,Qt::Horizontal,tr("事件序号"));
    sqlModel->setHeaderData(1,Qt::Horizontal,tr("类型"));
    sqlModel->setHeaderData(2,Qt::Horizontal,tr("X"));
    sqlModel->setHeaderData(3,Qt::Horizontal,tr("Y"));
    sqlModel->setHeaderData(4,Qt::Horizontal,tr("Z"));
    sqlModel->setHeaderData(5,Qt::Horizontal,tr("触发时间"));
    sqlModel->setHeaderData(6,Qt::Horizontal,tr("震级"));
    sqlModel->setHeaderData(7,Qt::Horizontal,tr("到时"));
    sqlModel->setHeaderData(8,Qt::Horizontal,tr("触发站台"));
    sqlModel->setHeaderData(9,Qt::Horizontal,tr("持续时间震级"));
    sqlModel->setHeaderData(10,Qt::Horizontal,tr("能量/J"));
    sqlModel->setHeaderData(11,Qt::Horizontal,tr("路径"));
    sqlModel->setHeaderData(12,Qt::Horizontal,tr("矩张量"));
    sqlModel->setHeaderData(13,Qt::Horizontal,tr("B值"));
    ui->dataBaseView->verticalHeader()->setVisible(false);            //禁止显示列标头
    //ui->dataBaseView->sortByColumn(0,Qt::DescendingOrder);          //按找事件序号降序排序
    ui->dataBaseView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->dataBaseView->setSelectionBehavior(QAbstractItemView::SelectRows);
    //ui->dataBaseView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->dataBaseView->setModel(sqlModel);

    QString latestDate;
    QSqlQuery query;
    query.exec( "select max(quackTime) from mine_quake_results;");            // 执行查询操作
    while(query.next()){
        latestDate = query.value(0).toString();
        qDebug()<<"date="<<latestDate;
    }
    ui->lastestIncidentLabel->setText("最新事件："+latestDate);

    db->close();                                                               //用完就关闭数据库
    delete db;
}

//鼠标双击dataBaseView(数据库视图)序号显示台站以及CAD定位点
void Widget::dataBaseViewDC(const QModelIndex &index)
{
    int row = ui->dataBaseView->currentIndex().row();

    QString str = ui->dataBaseView->model()->data(index).toString();
    QModelIndex tempIndex;
    QVariant data;

    if(currentDataBase == "mine_quake_results"){
        for(int i=0;i<3;i++){
            tempIndex = ui->dataBaseView->model()->index(row,i+2);       //获取CAD定位点
            data = ui->dataBaseView->model()->data(tempIndex);
            coordinates[i] = data.toDouble();
        }
        qDebug()<<"the current coordinate is :"<<coordinates[0]<<" "<<coordinates[1]<<" "<<coordinates[2];

        tempIndex = ui->dataBaseView->model()->index(row,11);            //获取CSV文件路径
        data = ui->dataBaseView->model()->data(tempIndex);
        QString filePath = data.toString();
        ReadCSVData::FILEPATH = filePath;
        qDebug()<<"the current csv file path is :"<<ReadCSVData::FILEPATH;
    }

    ConnectDataBase::SQLTABLE = currentDataBase;
    tempIndex = ui->dataBaseView->model()->index(row,0);                 //获取数据库事件ID
    ConnectDataBase::EVENTID  =ui->dataBaseView->model()->data(tempIndex).toInt();

    //画一个圆形，然后获取其ID，再动态闪烁
    QVariant result = ui->axWidget->dynamicCall("DrawCircle(double, double, double)",coordinates[0],coordinates[1],300);
    qlonglong id= result.toLongLong();
    ui->axWidget->dynamicCall("TwinkeEnt(qlonglong)",id);                                  
    rowCount.append(id);                                      //将该圆形实体ID存入容器，方便后期操作

    blinkId->enqueue(id);
    if(blinkId->size()>=2){
        qlonglong temp = blinkId->dequeue();
        ui->axWidget->dynamicCall("StopTwinkeEnt(qlonglong)",temp);
        ui->axWidget->dynamicCall("Clear(qlonglong)",temp);
    }
    globalStatusBar->showMessage(tr("读取文件：")+ReadCSVData::FILEPATH);
    emit pageSwitch9();

    mutex.lock();
    ReadCSVData *readData = new ReadCSVData();
    connect(readData, &ReadCSVData::finished, readData, &QObject::deleteLater);
    readData->start();
    mutex.unlock();
}

void Widget::on_currentIncidentButton_clicked()
{
    DrawThread *drawThread = new DrawThread();
    connect(drawThread, &DrawThread::finished, drawThread, &QObject::deleteLater);
    drawThread->start();

    QString currentStation ;
    for(int i=1;i<10;i++){
        if(ReadCSVData::TEMPMOTIPOS[i]!=0){
            currentStation += "T"+QString::number(i,10)+",";
        }
    }
    ui->currentIncidentStation->setText("当前台站："+currentStation);
    qDebug()<<"the current station is :"<<currentStation;

    globalStatusBar->showMessage(tr("显示图表"));
    //emit sendChartsYAxis();
}

void Widget::on_cleanAllChartDatas_clicked()
{
    emit cleanAllChartDatas();
}
