#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    filePath = "C:/Users/13696/Desktop/项目参考资料/wave/5moti/yzwu 2020-06-18 04-19-21`28.csv";
    //filePath = "D:/MicroquakeSystem_LiaoningUniversity/MicroquakeSystem_LiaoningUniversity/data/ConstructionData/3moti/suz 2020-06-15 09-17-48`07.csv";
    originListIndex = 0;         //一定要初始化，否则可能产生随机数
    isStoppingTimer = false;

    pageSwitchIndex = 0;

    db = new ConnectDataBase;
    db->conDataBase();

    readData = new ReadCSVData;
    readData->parseCSVFileName(filePath);
    readData->readCSVFile(filePath);
    readData->locateCSVData();

    initCharts();
    showStackedWidgetCharts();

    //以下为信号和槽函数，只写了一部分，剩余按钮用的自动关联
    connect(ui->displayButton,SIGNAL(clicked()),this,SLOT(displayButtonClick()));
    connect(ui->stopButton,SIGNAL(clicked()),this,SLOT(stopButtonClick()));
    connect(ui->fulllScreenButton,SIGNAL(clicked()),this,SLOT(fullChartsButtonClicked()));

    connect(ui->startButton,SIGNAL(clicked()),this,SLOT(startButtonClicked()));
    connect(ui->dailyStatement,SIGNAL(clicked()),this,SLOT(dailyStatementClicked()));
    connect(ui->monthlyStatement,SIGNAL(clicked()),this,SLOT(monthlyStatementClicked()));
    connect(ui->helpButton,SIGNAL(clicked()),this,SLOT(helpButtonClicked()));

    connect(ui->zoomIn,SIGNAL(clicked()),this,SLOT(zoomInClicked()));
    connect(ui->zoomOut,SIGNAL(clicked()),this,SLOT(zoomOutClicked()));
    connect(ui->restoreButton,SIGNAL(clicked()),this,SLOT(restoreButtonClicked()));
    connect(ui->moveViewButton,SIGNAL(clicked()),SLOT(moveViewButtonClicked()));

    connect(ui->dataBaseView,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(dataBaseViewDC(const QModelIndex &)));
}

Widget::~Widget()
{
    db->close();
    delete db;

    readData->~ReadCSVData();
    delete readData;

    delete splineSeries;
    delete scatterSeries;
    delete axisX;
    delete axisY;
    delete chart;
    delete view;

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

//在UI上显示数据库数据
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
    ui->dataBaseView->verticalHeader()->setVisible(false);     //禁止显示列标头
   // ui->tableView_2->sortByColumn(0,Qt::AscendingOrder);    //按列排序，升序
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
    //ui->axWidget->dynamicCall("SendStringToExecute(StopTwinkeEnt)");

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

//    MxDrawResbuf param ;
//    param.AddDouble(coordinates[0]);
//    param.AddDouble(coordinates[1]);

//    qDebug()<<"param->asVariant:"<<param.asVariant();

   //ui->axWidget->dynamicCall("SendStringToExecuteEx(QString sCmdName, QObject* pParam)","TwinkeEnt",param.asVariant());

    //IDispatch
   //QAxObject *filter = ui->axWidget->querySubObject("");
   MxDrawResbuf *filter = new MxDrawResbuf() ;
   QVariant p = ui->axWidget->dynamicCall("FindEntAtPoint(double dX, double dY, IDispatch* pFilter)",coordinates[0],coordinates[1],filter->asVariant());
   qDebug()<<"p="<<p;

   QVariant id = ui->axWidget->dynamicCall("DrawPoint(double dX, double dY)",coordinates[0],coordinates[1]);
   qDebug()<<"id="<<id;
   qDebug()<<"id="<<id.toLongLong();
   qlonglong q= id.toLongLong();
   ui->axWidget->dynamicCall("TwinkeEnt(qlonglong lId)",q);


    ui->stackedWidget->setCurrentIndex(9);                           //切换到台站波形图显示

}

//图表初始化
void Widget::initCharts()
{
    splineSeries = new QSplineSeries[27];
    lineSeries = new QLineSeries[27];
    scatterSeries = new QScatterSeries[27];
    chart = new QChart[27];
    axisX = new QValueAxis[27];
    axisY = new QValueAxis[27];
    view = new QChartView[27];

    splineSeries2 = new QSplineSeries[27];
    lineSeries2 = new QLineSeries[27];
    scatterSeries2 = new QScatterSeries[27];
    chart2 = new QChart[27];
    axisX2 = new QValueAxis[27];
    axisY2 = new QValueAxis[27];
    view2 = new QChartView[27];

    //QMargins margin(10,10,5,5);                           //设置chart rectangle（图表矩形）的边距
    QRectF recf(20,5,260,50);                            //显示设置plot area（图表区域）的大小

    for (int i=0;i<27;i++) {
        splineSeries[i].setColor(QColor(Qt::black));
        splineSeries[i].setUseOpenGL(true);
        splineSeries[i].setPointsVisible(true);
        scatterSeries[i].setMarkerSize(8);
        lineSeries[i].setColor(QColor(255,0,0));

        chart[i].legend()->hide();
        chart[i].addSeries(&splineSeries[i]);            //为图表添加曲线序列
        chart[i].addSeries(&lineSeries[i]);              //为图标添加折线序列
        chart[i].addSeries(&scatterSeries[i]);           //为图表添加点序列

        axisX[i].setRange(0, 90000);                     //设置坐标轴范围
        axisX[i].setTitleText("time(0.2millisecs)");     //标题
        axisX[i].setTickCount(10);                       //9区域，10个刻度
        axisX[i].setMinorTickCount(1);                   //单位刻度中间再加一个副刻度

        axisY[i].setRange(-50000, 50000);
        axisY[i].setTitleText("accelerated speed");
        axisY[i].setTickCount(5);

        chart[i].addAxis(&axisX[i], Qt::AlignBottom);    //把坐标轴添加到chart中，第二个参数是设置坐标轴的位置，
        splineSeries[i].attachAxis(&axisX[i]);           //把曲线关联到坐标轴
        scatterSeries[i].attachAxis(&axisX[i]);
        lineSeries[i].attachAxis(&axisX[i]);

        chart[i].addAxis(&axisY[i], Qt::AlignLeft);
        splineSeries[i].attachAxis(&axisY[i]);
        lineSeries[i].attachAxis(&axisY[i]);
        scatterSeries[i].attachAxis(&axisY[i]);

        view[i].setChart(&chart[i]);
        view[i].setRubberBand(QChartView::RectangleRubberBand);
        view[i].installEventFilter(this);                //注册部件事件过滤
    }

    for (int i=0;i<27;i++) {
        splineSeries2[i].setColor(QColor(Qt::black));
        splineSeries2[i].setUseOpenGL(true);
        splineSeries2[i].setPointsVisible(true);
        scatterSeries2[i].setMarkerSize(3);
        lineSeries2[i].setColor(QColor(255,0,0));

        chart2[i].legend()->hide();
        chart2[i].addSeries(&splineSeries2[i]);            //为图表添加曲线序列
        chart2[i].addSeries(&scatterSeries2[i]);           //为图表添加点序列
        chart2[i].addSeries(&lineSeries2[i]);
        chart2[i].setTheme(QChart::ChartThemeDark);
        //chart2[i].setBackgroundVisible(false);
        //chart2[i].setMargins(margin);
        chart2[i].setPlotArea(recf);

        axisX2[i].setRange(0, 90000);                     //设置坐标轴范围
        axisX2[i].setTickCount(10);                       //9区域，10个刻度
        axisY2[i].setRange(-50000, 50000);

        chart2[i].addAxis(&axisX2[i], Qt::AlignBottom);    //把坐标轴添加到chart中，第二个参数是设置坐标轴的位置，
        splineSeries2[i].attachAxis(&axisX2[i]);           //把曲线关联到坐标轴
        scatterSeries2[i].attachAxis(&axisX2[i]);
        lineSeries2[i].attachAxis(&axisX2[i]);

        chart2[i].addAxis(&axisY2[i], Qt::AlignLeft);
        splineSeries2[i].attachAxis(&axisY2[i]);
        scatterSeries2[i].attachAxis(&axisY2[i]);
        lineSeries2[i].attachAxis(&axisY2[i]);

        view2[i].setChart(&chart2[i]);
        view2[i].setRubberBand(QChartView::RectangleRubberBand);
        view2[i].installEventFilter(this);                //注册部件事件过滤
    }
}

//将标签和view[T]图表添加到stackWidget的GridLayout布局中
void Widget::showStackedWidgetCharts()
{
    ui->gridLayout->addWidget(ui->t1,1,0);
    ui->gridLayout->addWidget(ui->t1X,0,1);
    ui->gridLayout->addWidget(&view[T1X],0,2);
    ui->gridLayout->addWidget(ui->t1Y,1,1);
    ui->gridLayout->addWidget(&view[T1Y],1,2);
    ui->gridLayout->addWidget(ui->t1Z,2,1);
    ui->gridLayout->addWidget(&view[T1Z],2,2);

    ui->gridLayout_2->addWidget(ui->t2,1,0);
    ui->gridLayout_2->addWidget(ui->t2X,0,1);
    ui->gridLayout_2->addWidget(&view[T2X],0,2);
    ui->gridLayout_2->addWidget(ui->t2Y,1,1);
    ui->gridLayout_2->addWidget(&view[T2Y],1,2);
    ui->gridLayout_2->addWidget(ui->t2Z,2,1);
    ui->gridLayout_2->addWidget(&view[T2Z],2,2);

    ui->gridLayout_3->addWidget(ui->t3,1,0);
    ui->gridLayout_3->addWidget(ui->t3X,0,1);
    ui->gridLayout_3->addWidget(&view[T3X],0,2);
    ui->gridLayout_3->addWidget(ui->t3Y,1,1);
    ui->gridLayout_3->addWidget(&view[T3Y],1,2);
    ui->gridLayout_3->addWidget(ui->t3Z,2,1);
    ui->gridLayout_3->addWidget(&view[T3Z],2,2);

    ui->gridLayout_4->addWidget(ui->t4,1,0);
    ui->gridLayout_4->addWidget(ui->t4X,0,1);
    ui->gridLayout_4->addWidget(&view[T4X],0,2);
    ui->gridLayout_4->addWidget(ui->t4Y,1,1);
    ui->gridLayout_4->addWidget(&view[T4Y],1,2);
    ui->gridLayout_4->addWidget(ui->t4Z,2,1);
    ui->gridLayout_4->addWidget(&view[T4Z],2,2);

    ui->gridLayout_5->addWidget(ui->t5,1,0);
    ui->gridLayout_5->addWidget(ui->t5X,0,1);
    ui->gridLayout_5->addWidget(&view[T5X],0,2);
    ui->gridLayout_5->addWidget(ui->t5Y,1,1);
    ui->gridLayout_5->addWidget(&view[T5Y],1,2);
    ui->gridLayout_5->addWidget(ui->t5Z,2,1);
    ui->gridLayout_5->addWidget(&view[T5Z],2,2);

    ui->gridLayout_6->addWidget(ui->t6,1,0);
    ui->gridLayout_6->addWidget(ui->t6X,0,1);
    ui->gridLayout_6->addWidget(&view[T6X],0,2);
    ui->gridLayout_6->addWidget(ui->t6Y,1,1);
    ui->gridLayout_6->addWidget(&view[T6Y],1,2);
    ui->gridLayout_6->addWidget(ui->t6Z,2,1);
    ui->gridLayout_6->addWidget(&view[T6Z],2,2);

    ui->gridLayout_7->addWidget(ui->t7,1,0);
    ui->gridLayout_7->addWidget(ui->t7X,0,1);
    ui->gridLayout_7->addWidget(&view[T7X],0,2);
    ui->gridLayout_7->addWidget(ui->t7Y,1,1);
    ui->gridLayout_7->addWidget(&view[T7Y],1,2);
    ui->gridLayout_7->addWidget(ui->t7Z,2,1);
    ui->gridLayout_7->addWidget(&view[T7Z],2,2);

    ui->gridLayout_8->addWidget(ui->t8,1,0);
    ui->gridLayout_8->addWidget(ui->t8X,0,1);
    ui->gridLayout_8->addWidget(&view[T8X],0,2);
    ui->gridLayout_8->addWidget(ui->t8Y,1,1);
    ui->gridLayout_8->addWidget(&view[T8Y],1,2);
    ui->gridLayout_8->addWidget(ui->t8Z,2,1);
    ui->gridLayout_8->addWidget(&view[T8Z],2,2);

    ui->gridLayout_9->addWidget(ui->t9,1,0);
    ui->gridLayout_9->addWidget(ui->t9X,0,1);
    ui->gridLayout_9->addWidget(&view[T9X],0,2);
    ui->gridLayout_9->addWidget(ui->t9Y,1,1);
    ui->gridLayout_9->addWidget(&view[T9Y],1,2);
    ui->gridLayout_9->addWidget(ui->t9Z,2,1);
    ui->gridLayout_9->addWidget(&view[T9Z],2,2);


    ui->gridLayout_10->addWidget(&view2[T1X],0,0);
    ui->gridLayout_10->addWidget(&view2[T1Y],0,1);
    ui->gridLayout_10->addWidget(&view2[T1Z],0,2);
    ui->gridLayout_10->addWidget(&view2[T2X],1,0);
    ui->gridLayout_10->addWidget(&view2[T2Y],1,1);
    ui->gridLayout_10->addWidget(&view2[T2Z],1,2);
    ui->gridLayout_10->addWidget(&view2[T3X],2,0);
    ui->gridLayout_10->addWidget(&view2[T3Y],2,1);
    ui->gridLayout_10->addWidget(&view2[T3Z],2,2);
    ui->gridLayout_10->addWidget(&view2[T4X],3,0);
    ui->gridLayout_10->addWidget(&view2[T4Y],3,1);
    ui->gridLayout_10->addWidget(&view2[T4Z],3,2);
    ui->gridLayout_10->addWidget(&view2[T5X],4,0);
    ui->gridLayout_10->addWidget(&view2[T5Y],4,1);
    ui->gridLayout_10->addWidget(&view2[T5Z],4,2);
    ui->gridLayout_10->addWidget(&view2[T6X],5,0);
    ui->gridLayout_10->addWidget(&view2[T6Y],5,1);
    ui->gridLayout_10->addWidget(&view2[T6Z],5,2);
    ui->gridLayout_10->addWidget(&view2[T7X],6,0);
    ui->gridLayout_10->addWidget(&view2[T7Y],6,1);
    ui->gridLayout_10->addWidget(&view2[T7Z],6,2);
    ui->gridLayout_10->addWidget(&view2[T8X],7,0);
    ui->gridLayout_10->addWidget(&view2[T8Y],7,1);
    ui->gridLayout_10->addWidget(&view2[T8Z],7,2);
    ui->gridLayout_10->addWidget(&view2[T9X],8,0);
    ui->gridLayout_10->addWidget(&view2[T9Y],8,1);
    ui->gridLayout_10->addWidget(&view2[T9Z],8,2);

    ui->stackedWidget->setCurrentIndex(0);
}

//T1~T9按钮，用来显示不同站台曲线图
void Widget::on_T1Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
void Widget::on_T2Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}
void Widget::on_T3Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}
void Widget::on_T4Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}
void Widget::on_T5Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}
void Widget::on_T6Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}
void Widget::on_T7Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
}
void Widget::on_T8Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(7);
}
void Widget::on_T9Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(8);
}

//下一页
void Widget::on_nextPage_clicked()
{
     pageSwitchIndex++;
     if(pageSwitchIndex>9){
         pageSwitchIndex = 0;
     }
     qDebug()<<"pageSwitchIndex = "<<pageSwitchIndex<<'\n';
     ui->stackedWidget->setCurrentIndex(pageSwitchIndex);

}
//上一页
void Widget::on_previousPage_clicked()
{
    pageSwitchIndex--;
    if(pageSwitchIndex < 0){
        pageSwitchIndex = 9;
    }
    qDebug()<<"pageSwitchIndex = "<<pageSwitchIndex<<'\n';
    ui->stackedWidget->setCurrentIndex(pageSwitchIndex);
}

//显示按钮，启动定时器
void Widget::displayButtonClick()
{
    drawTimer = new QTimer(this);                       //定时任务
//   connect(drawTimer, SIGNAL(timeout()), this, SLOT(drawSplineWave());
//    drawTimer->start(10);                                //10ms执行一次

    QString station;
    for(int i=0;i<readData->senNum;i++){
        station.append(readData->senChannelNum[0][i]+' ');
    }
    ui->currentIncidentStation->setText("当前台站："+station);

    drawSplineWave();
}
//暂停按钮，停止定时器
void Widget::stopButtonClick()
{
    if (QObject::sender() == ui->stopButton) {
        if (!isStoppingTimer) {
            drawTimer->stop();
        } else {
            drawTimer->start();
        }
        isStoppingTimer = !isStoppingTimer;
    }
}
//全部按钮，显示T1~T9站台全部曲线图信息
void Widget::fullChartsButtonClicked()
{
    qDebug()<<"pageSwitchIndex = "<<pageSwitchIndex<<'\n';
    ui->stackedWidget->setCurrentIndex(9);
}


//绘制曲线图
void Widget::drawSplineWave()
{

    for(int i=0;i<27;i++){
        splineSeries[i].replace(readData->pointBuffer[i]);
    }

    for(int i=0;i<27;i++){
        splineSeries2[i].replace(readData->pointBuffer[i]);
    }

    //根据readData->tempMotiPos[j][0]获取站台及激发位置，然后将其绘制到图表中（P波到时）
    for(int j=0;j<10;j++){
        if(readData->tempMotiPos[j][0]!=0){
            if(1==j){                 //如果站台是1站台，则将P波到时绘制到T1的XYZ轴曲线图表中
                lineSeries[T1X].replace(readData->pointBuffer_P[j]);
                lineSeries[T1Y].replace(readData->pointBuffer_P[j]);
                lineSeries[T1Z].replace(readData->pointBuffer_P[j]);

                lineSeries2[T1X].replace(readData->pointBuffer_P[j]);
                lineSeries2[T1Y].replace(readData->pointBuffer_P[j]);
                lineSeries2[T1Z].replace(readData->pointBuffer_P[j]);
            }
            if(2==j){
                lineSeries[T2X].replace(readData->pointBuffer_P[j]);
                lineSeries[T2Y].replace(readData->pointBuffer_P[j]);
                lineSeries[T2Z].replace(readData->pointBuffer_P[j]);

                lineSeries2[T2X].replace(readData->pointBuffer_P[j]);
                lineSeries2[T2Y].replace(readData->pointBuffer_P[j]);
                lineSeries2[T2Z].replace(readData->pointBuffer_P[j]);
            }
            if(3==j){
                lineSeries[T3X].replace(readData->pointBuffer_P[j]);
                lineSeries[T3Y].replace(readData->pointBuffer_P[j]);
                lineSeries[T3Z].replace(readData->pointBuffer_P[j]);

                lineSeries2[T3X].replace(readData->pointBuffer_P[j]);
                lineSeries2[T3Y].replace(readData->pointBuffer_P[j]);
                lineSeries2[T3Z].replace(readData->pointBuffer_P[j]);
            }
            if(4==j){
                lineSeries[T4X].replace(readData->pointBuffer_P[j]);
                lineSeries[T4Y].replace(readData->pointBuffer_P[j]);
                lineSeries[T4Z].replace(readData->pointBuffer_P[j]);

                lineSeries2[T4X].replace(readData->pointBuffer_P[j]);
                lineSeries2[T4Y].replace(readData->pointBuffer_P[j]);
                lineSeries2[T4Z].replace(readData->pointBuffer_P[j]);
            }
            if(5==j){
                lineSeries[T5X].replace(readData->pointBuffer_P[j]);
                lineSeries[T5Y].replace(readData->pointBuffer_P[j]);
                lineSeries[T5Z].replace(readData->pointBuffer_P[j]);

                lineSeries2[T5X].replace(readData->pointBuffer_P[j]);
                lineSeries2[T5Y].replace(readData->pointBuffer_P[j]);
                lineSeries2[T5Z].replace(readData->pointBuffer_P[j]);
            }
            if(6==j){
                lineSeries[T6X].replace(readData->pointBuffer_P[j]);
                lineSeries[T6Y].replace(readData->pointBuffer_P[j]);
                lineSeries[T6Z].replace(readData->pointBuffer_P[j]);

                lineSeries2[T6X].replace(readData->pointBuffer_P[j]);
                lineSeries2[T6Y].replace(readData->pointBuffer_P[j]);
                lineSeries2[T6Z].replace(readData->pointBuffer_P[j]);
            }
            if(7==j){
                lineSeries[T7X].replace(readData->pointBuffer_P[j]);
                lineSeries[T7Y].replace(readData->pointBuffer_P[j]);
                lineSeries[T7Z].replace(readData->pointBuffer_P[j]);

                lineSeries2[T7X].replace(readData->pointBuffer_P[j]);
                lineSeries2[T7Y].replace(readData->pointBuffer_P[j]);
                lineSeries2[T7Z].replace(readData->pointBuffer_P[j]);
            }
            if(8==j){
                lineSeries[T8X].replace(readData->pointBuffer_P[j]);
                lineSeries[T8Y].replace(readData->pointBuffer_P[j]);
                lineSeries[T8Z].replace(readData->pointBuffer_P[j]);

                lineSeries2[T8X].replace(readData->pointBuffer_P[j]);
                lineSeries2[T8Y].replace(readData->pointBuffer_P[j]);
                lineSeries2[T8Z].replace(readData->pointBuffer_P[j]);
            }
            if(9==j){
                lineSeries[T9X].replace(readData->pointBuffer_P[j]);
                lineSeries[T9Y].replace(readData->pointBuffer_P[j]);
                lineSeries[T9Z].replace(readData->pointBuffer_P[j]);

                lineSeries2[T9X].replace(readData->pointBuffer_P[j]);
                lineSeries2[T9Y].replace(readData->pointBuffer_P[j]);
                lineSeries2[T9Z].replace(readData->pointBuffer_P[j]);
            }
        }
    }
}

//chartView事件过滤封装
void Widget::charViewEventFilter(QEvent *event,QChart *tempChart)
{
    if(event->type()==QEvent::Wheel){
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event); //将event强制转换为发生的事件的类型
        if(wheelEvent->delta()>0)tempChart->zoom(1.1);
        else tempChart->zoom(10.0/11);
    }
    else if(event->type()==QEvent::MouseButtonPress){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if(mouseEvent->button() & Qt::RightButton)
            tempChart->zoomReset();
        else if (mouseEvent->button() & Qt::LeftButton)isClickingChart = true;
    }
    else if(event->type()==QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        switch (keyEvent->key()) {
        case Qt::Key_Left:
            tempChart->scroll(-10, 0);
            break;
        case Qt::Key_Right:
            tempChart->scroll(10, 0);
            break;
        case Qt::Key_Up:
            tempChart->scroll(0, 10);
            break;
        case Qt::Key_Down:
            tempChart->scroll(0, -10);
            break;
        default:
            keyPressEvent(keyEvent);
            break;
        }
    }
}

//事件过滤
bool Widget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == &view[T1X]){
        charViewEventFilter(event,&chart[T1X]);
    }
    if(obj == &view[T1Y]){
        charViewEventFilter(event,&chart[T1Y]);
    }
    if(obj == &view[T1Z]){
        charViewEventFilter(event,&chart[T1Z]);
    }
    if(obj == &view[T2X]){
        charViewEventFilter(event,&chart[T2X]);
    }
    if(obj == &view[T2Y]){
        charViewEventFilter(event,&chart[T2Y]);
    }
    if(obj == &view[T2Z]){
        charViewEventFilter(event,&chart[T2Z]);
    }
    if(obj == &view[T3X]){
        charViewEventFilter(event,&chart[T3X]);
    }
    if(obj == &view[T3Y]){
        charViewEventFilter(event,&chart[T3Y]);
    }
    if(obj == &view[T3Z]){
        charViewEventFilter(event,&chart[T3Z]);
    }
    if(obj == &view[T4X]){
        charViewEventFilter(event,&chart[T4X]);
    }
    if(obj == &view[T4Y]){
        charViewEventFilter(event,&chart[T4Y]);
    }
    if(obj == &view[T4Z]){
        charViewEventFilter(event,&chart[T4Z]);
    }
    if(obj == &view[T5X]){
        charViewEventFilter(event,&chart[T5X]);
    }
    if(obj == &view[T5Y]){
        charViewEventFilter(event,&chart[T5Y]);
    }
    if(obj == &view[T5Z]){
        charViewEventFilter(event,&chart[T5Z]);
    }
    if(obj == &view[T6X]){
        charViewEventFilter(event,&chart[T6X]);
    }
    if(obj == &view[T6Y]){
        charViewEventFilter(event,&chart[T6Y]);
    }
    if(obj == &view[T6Z]){
        charViewEventFilter(event,&chart[T6Z]);
    }
    if(obj == &view[T7X]){
        charViewEventFilter(event,&chart[T7X]);
    }
    if(obj == &view[T7Y]){
        charViewEventFilter(event,&chart[T7Y]);
    }
    if(obj == &view[T7Z]){
        charViewEventFilter(event,&chart[T7Z]);
    }
    if(obj == &view[T8X]){
        charViewEventFilter(event,&chart[T8X]);
    }
    if(obj == &view[T8Y]){
        charViewEventFilter(event,&chart[T8Y]);
    }
    if(obj == &view[T8Z]){
        charViewEventFilter(event,&chart[T8Z]);
    }
    if(obj == &view[T9X]){
        charViewEventFilter(event,&chart[T9X]);
    }
    if(obj == &view[T9Y]){
        charViewEventFilter(event,&chart[T9Y]);
    }
    if(obj == &view[T9Z]){
        charViewEventFilter(event,&chart[T9Z]);
    }
    else return QWidget::eventFilter(obj,event);
}

//鼠标移动事件
void Widget::mouseMoveEvent(QMouseEvent *event)
{
    int x, y;
    if (isClickingChart) {
        if (xOld == 0 && yOld == 0) {

        } else {
            x = event->x() - xOld;
            y = event->y() - yOld;
            chart->scroll(-x, y);
        }
        xOld = event->x();
        yOld = event->y();
        return;
    }
}

//鼠标释放事件
void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    if (isClickingChart) {
        xOld = yOld = 0;
        isClickingChart = false;
    }
}

