#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    db = new ConnectDataBase;
    db->conDataBase();

    showChart = new ShowChart(this);
    ui->scrollArea->setWidget(showChart);
    dw = new DynamicWave;
    //dw->setWindowFlags(dw->windowFlags()& ~Qt::WindowCloseButtonHint);
    connect(ui->startButton,SIGNAL(clicked()),this,SLOT(startButtonClicked()));
    connect(ui->dailyStatement,SIGNAL(clicked()),this,SLOT(dailyStatementClicked()));
    connect(ui->monthlyStatement,SIGNAL(clicked()),this,SLOT(monthlyStatementClicked()));
    connect(ui->helpButton,SIGNAL(clicked()),this,SLOT(helpButtonClicked()));

    connect(ui->zoomIn,SIGNAL(clicked()),this,SLOT(zoomInClicked()));
    connect(ui->zoomOut,SIGNAL(clicked()),this,SLOT(zoomOutClicked()));
    connect(ui->restoreButton,SIGNAL(clicked()),this,SLOT(restoreButtonClicked()));
    connect(ui->moveViewButton,SIGNAL(clicked()),SLOT(moveViewButtonClicked()));
    connect(ui->cancelDynBlink,SIGNAL(clicked()),SLOT(cancleDynBlinkClicked()));
    connect(ui->dynamicWaveButton,SIGNAL(clicked()),this,SLOT(dynamicWaveButtonClick()));

    connect(ui->dataBaseView,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(dataBaseViewDC(const QModelIndex &)));
    connect(this,SIGNAL(pageSwitch9()),showChart,SLOT(pageSwithTo9()));
    connect(this,SIGNAL(sendSelectedCSVFile(QString)),showChart,SLOT(receiveCSVFilePath(QString)));

    connect(this,SIGNAL(sendSelectedCSVFile(QString)),dw,SLOT(receiveCSVFilePath2(QString)));

}

Widget::~Widget()
{
    db->close();
    delete db;
    delete showChart;
    delete dw;
    delete ui;
}

//控件命令按钮
void Widget::on_axWidget_ImplementCommandEvent(int iCommandId)
{
    if(iCommandId == 1){
        // 调用控件打开dwg文件命令。
        ui->axWidget->dynamicCall("OpenDwgFile(const QString&)","C:\\Users\\13696\\Desktop\\项目参考资料\\红阳三矿20200713lh.dwg");
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
        ui->axWidget->dynamicCall("StopAllTwinkeEnt()");
        for(int row=0;row<entityId.size();row++)
            if(entityId.contains(row)){
                QList<qlonglong> myValues = entityId.values(row);
                 for (int i=0;i<myValues.size();i++) {
                     ui->axWidget->dynamicCall("Clear(qlonglong)",myValues.at(i));
                     qDebug()<<"values="<<myValues.at(i);
                 }
            }
    }
}

//运行按钮
void Widget::startButtonClicked()
{ 
    ui->startButton->setEnabled(false);                      //运行按钮点击一次后就禁用

    ui->axWidget->dynamicCall("DoCommand(const qint32&)",1); //执行控件自定义命令函数，命令的id为2，该值为命令编号，可任取.
    showTable();
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
//显示动态波形图，
void Widget::dynamicWaveButtonClick()
{
    if(dw == nullptr){
        dw = new DynamicWave;
        dw->showMaximized();
    }
    else
        dw->showMaximized();
}
//在UI上显示数据库数据
void Widget::showTable()
{
    //QStandardItemModel* m_list_item_model = NULL;
    //m_list_item_model->setColumnCount(6);//设置列表列数
    QSqlQueryModel * model = new QSqlQueryModel(this);
    model->setQuery("select * from basicdata");
    model->setHeaderData(0,Qt::Horizontal,tr("事件序号"));
    model->setHeaderData(1,Qt::Horizontal,tr("触发时间"));
    model->setHeaderData(2,Qt::Horizontal,tr("触发站台"));
    model->setHeaderData(3,Qt::Horizontal,tr("定位坐标"));
    model->setHeaderData(4,Qt::Horizontal,tr("能量/J"));
    model->setHeaderData(5,Qt::Horizontal,tr("震级"));
    ui->dataBaseView->verticalHeader()->setVisible(false);     //禁止显示列标头
   // ui->tableView_2->sortByColumn(0,Qt::AscendingOrder);     //按列排序，升序
    ui->dataBaseView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->dataBaseView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->dataBaseView->setModel(model);

    QAbstractItemModel *itemModel = ui->dataBaseView->model();
    QModelIndex itemIndex = itemModel->index(0,1);            //定位到事件1，获取其时间
    QVariant data = itemModel->data(itemIndex);
    ui->lastestIncidentLabel->setText("最新事件："+data.toString());
}

//鼠标双击dataBaseView(数据库视图)序号显示台站以及CAD定位点
void Widget::dataBaseViewDC(const QModelIndex &index)
{
    double *coordinates = new double[3];
    int row = ui->dataBaseView->currentIndex().row();
    qDebug()<<"the selected row is:"<<row;

    QString str = ui->dataBaseView->model()->data(index).toString();
    qDebug()<<"the selected str is :"<<str;

    QModelIndex tempIndex =ui->dataBaseView->model()->index(row,2);  //选中行第三列的内容，即台站名称
    QVariant data =ui->dataBaseView->model()->data(tempIndex);
    QString currentStation = data.toString();
    ui->currentIncidentStation->setText("当前台站："+currentStation);
    qDebug()<<"the current station is :"<<currentStation;

    tempIndex = ui->dataBaseView->model()->index(row,3);             //获取CAD定位点
    data = ui->dataBaseView->model()->data(tempIndex);
    QStringList list = data.toString().split(",");
    for(int i=0;i<list.size();i++){
        coordinates[i] = list.at(i).toDouble();
    }
    qDebug()<<"the current coordinate is :"<<coordinates[0]<<" "<<coordinates[1]<<" "<<coordinates[2];

    tempIndex = ui->dataBaseView->model()->index(row,6);            //获取CSV文件路径
    data = ui->dataBaseView->model()->data(tempIndex);
    QString filePath = data.toString();
    qDebug()<<"the current csv file path is :"<<filePath;

    //画一个圆形，然后获取其ID，再动态闪烁，效果可以
    QVariant result = ui->axWidget->dynamicCall("DrawCircle(double, double, double)",coordinates[0],coordinates[1],300);
    qDebug()<<"id="<<result.toLongLong();
    qlonglong id= result.toLongLong();
    ui->axWidget->dynamicCall("TwinkeEnt(qlonglong)",id);

    entityId.insertMulti(row,id);                        //将该圆形实体ID存入容器，方便后期操作

      //下面是生成QT版的com接口说明文档，很有必要阅读
//    QString doc=ui->axWidget->generateDocumentation();
//    QFile outFile("c:/Users/13696/Desktop/项目参考资料/debuglog.html");
//    outFile.open(QIODevice::WriteOnly|QIODevice::Append);
//    QTextStream ts(&outFile);
//    ts<<doc<<endl;
//    outFile.close();

    emit sendSelectedCSVFile(filePath);
    emit pageSwitch9();
}


