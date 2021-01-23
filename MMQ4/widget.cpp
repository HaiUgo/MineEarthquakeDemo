#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->refreshCurrentDataTable->setEnabled(false);

    userInput = 0;
    filePath = "";

    initCharts();

    miningAreaMap = MiningAreaMap::getMiningAreaMapConfigure();
    ui->whichRegion->addItems(miningAreaMap.keys());
    miningAreaLocation = MiningAreaLocation::getMiningAreaLocationConfigure(ui->whichRegion->currentText());
    LocationAlgorithm::REGION = ui->whichRegion->currentText();
    whichRegion = miningAreaMap.value(ui->whichRegion->currentText());

    if(DataBaseConfigure::getDataBaseConfigure()){
        datatable = DataBaseConfigure::table;
        ui->databaseSoureComboBox->addItem(datatable);
        db = new ConnectDataBase;
        db->conDataBase();
    }

    blinkId = new QQueue<qlonglong>();

    globalStatusBar = new QStatusBar(this);
    layout()->addWidget(globalStatusBar);
    globalStatusBar->showMessage(tr("欢迎来到矿山高频微震地面台网监控系统！"));

    sqlModel = new QSqlQueryModel(this);

    //对输入框做限制，只能输入数字，长度为5
    QRegExp regx("[0-9]+$");
    QValidator *validator=new QRegExpValidator(regx,this);
    ui->intputPWave->setValidator(validator);
    ui->intputPWave->setMaxLength(5);

    connect(ui->startButton,SIGNAL(clicked()),this,SLOT(startButtonClicked()));
    connect(ui->dailyStatement,SIGNAL(clicked()),this,SLOT(dailyStatementClicked()));
    connect(ui->helpButton,SIGNAL(clicked()),this,SLOT(helpButtonClicked()));
    connect(ui->zoomIn,SIGNAL(clicked()),this,SLOT(zoomInClicked()));
    connect(ui->zoomOut,SIGNAL(clicked()),this,SLOT(zoomOutClicked()));
    connect(ui->restoreButton,SIGNAL(clicked()),this,SLOT(restoreButtonClicked()));
    connect(ui->moveViewButton,SIGNAL(clicked()),SLOT(moveViewButtonClicked()));
    connect(ui->cancelDynBlink,SIGNAL(clicked()),SLOT(cancleDynBlinkClicked()));
    connect(ui->refreshCurrentDataTable,SIGNAL(clicked()),SLOT(refreshCurrentDataTable()));
    connect(ui->dataBaseView,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(dataBaseViewDC(const QModelIndex &)));
    connect(ui->whichRegion, QOverload<const QString &>::of(&QComboBox::currentIndexChanged),
          [=](const QString &text){
        whichRegion = miningAreaMap.value(text);
        LocationAlgorithm::REGION = ui->whichRegion->currentText();
        ui->axWidget->dynamicCall("DoCommand(const qint32&)",1);
    });

    //以下为信号和槽函数，只写了一部分，剩余按钮用的自动关联
    connect(ui->fulllScreenButton,SIGNAL(clicked()),this,SLOT(fullChartsButtonClicked()));
    connect(ui->saveModifiedPWave,SIGNAL(clicked()),this,SLOT(informationDialog()));
    connect(ui->intputPWave,&QLineEdit::returnPressed,this,&Widget::repaintPWave);
    connect(ui->locationAlgorithmButton,SIGNAL(clicked()),this,SLOT(getLoactionData()));
    connect(ui->comboBox,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&Widget::getCurrentMotipos);

    for(int i=0;i<27;i++){
        connect(&splineSeries[i], &QSplineSeries::hovered, this, &Widget::slotPointHoverd);//用于鼠标移动到点上显示数值
        connect(&lineSeries[i], &QLineSeries::hovered, this, &Widget::slotPointHoverd);//用于鼠标移动到点上显示数值
    }
}

Widget::~Widget()
{
    delete blinkId;
    delete ui;
    qDebug()<<"Widget::~Widget()";
}

//图表初始化
void Widget::initCharts()
{
    splineSeries = new QSplineSeries[27];
    lineSeries = new QLineSeries[27];
    chart = new Chart[27];
    axisX = new QValueAxis[27];
    axisY = new QValueAxis[27];
    view = new ChartView [27];

    splineSeries2 = new QSplineSeries[9];
    lineSeries2 = new QLineSeries[9];
    chart2 = new Chart[9];
    axisX2 = new QValueAxis[9];
    axisY2 = new QValueAxis[9];
    view2 = new ChartView[9];

    m_valueLabel = new QLabel(this);

    QMargins margin(10,10,5,5);                           //设置chart rectangle（图表矩形）的边距
    //QRectF recf(10,10,233,60);

    for (int i=0;i<27;i++) {
        splineSeries[i].setColor(QColor(Qt::black));
        splineSeries[i].setUseOpenGL(true);
        splineSeries[i].setPointsVisible(true);
        lineSeries[i].setColor(QColor(255,0,0));
        chart[i].legend()->hide();
        chart[i].addSeries(&splineSeries[i]);              //为图表添加曲线序列
        chart[i].addSeries(&lineSeries[i]);                //为图标添加折线序列

        axisX[i].setRange(0, 9000);                       //设置坐标轴范围
        axisX[i].setTitleText("sampling point");
        axisX[i].setTickCount(10);                         //9区域，10个刻度
        axisX[i].setMinorTickCount(1);                     //单位刻度中间再加一个副刻度

        axisY[i].setRange(-20000, 20000);
        axisY[i].setTitleText("accelerated speed");
        axisY[i].setTickCount(5);

        chart[i].addAxis(&axisX[i], Qt::AlignBottom);      //把坐标轴添加到chart中，第二个参数是设置坐标轴的位置，
        splineSeries[i].attachAxis(&axisX[i]);             //把曲线关联到坐标轴
        lineSeries[i].attachAxis(&axisX[i]);

        chart[i].addAxis(&axisY[i], Qt::AlignLeft);
        splineSeries[i].attachAxis(&axisY[i]);
        lineSeries[i].attachAxis(&axisY[i]);

        view[i].setChart(&chart[i]);
        view[i].setRubberBand(QChartView::RectangleRubberBand);
        view[i].installEventFilter(this);                  //注册部件事件过滤
    }
    qDebug()<<"checka";
    for (int i=0;i<9;i++) {
        splineSeries2[i].setColor(QColor(Qt::black));
        splineSeries2[i].setUseOpenGL(true);
        splineSeries2[i].setPointsVisible(false);
        lineSeries2[i].setColor(QColor(255,0,0));
        lineSeries2[i].setPointLabelsFormat("(@xPoint)");
        chart2[i].legend()->hide();
        chart2[i].addSeries(&splineSeries2[i]);            //为图表添加曲线序列
        chart2[i].addSeries(&lineSeries2[i]);
        chart2[i].setTheme(QChart::ChartThemeLight);
        chart2[i].setBackgroundVisible(false);
        chart2[i].setMargins(margin);
       // chart2[i].setPlotArea(recf);

        axisX2[i].setRange(0, 9000);                      //设置坐标轴范围
        axisX2[i].setTickCount(10);
        axisY2[i].setRange(-20000, 20000);

        chart2[i].addAxis(&axisX2[i], Qt::AlignBottom);    //把坐标轴添加到chart中，第二个参数是设置坐标轴的位置，
        splineSeries2[i].attachAxis(&axisX2[i]);           //把曲线关联到坐标轴
        lineSeries2[i].attachAxis(&axisX2[i]);

        chart2[i].addAxis(&axisY2[i], Qt::AlignLeft);
        splineSeries2[i].attachAxis(&axisY2[i]);
        lineSeries2[i].attachAxis(&axisY2[i]);

        view2[i].setChart(&chart2[i]);
        view2[i].installEventFilter(this);                //注册部件事件过滤
    }
    ui->gridLayout_1->addWidget(ui->t1,1,0);
    ui->gridLayout_1->addWidget(ui->t1X,0,1);
    ui->gridLayout_1->addWidget(&view[T1X],0,2);
    ui->gridLayout_1->addWidget(ui->t1Y,1,1);
    ui->gridLayout_1->addWidget(&view[T1Y],1,2);
    ui->gridLayout_1->addWidget(ui->t1Z,2,1);
    ui->gridLayout_1->addWidget(&view[T1Z],2,2);

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

    ui->gridLayout_5->addWidget(ui->t4,1,0);
    ui->gridLayout_5->addWidget(ui->t4X,0,1);
    ui->gridLayout_5->addWidget(&view[T4X],0,2);
    ui->gridLayout_5->addWidget(ui->t4Y,1,1);
    ui->gridLayout_5->addWidget(&view[T4Y],1,2);
    ui->gridLayout_5->addWidget(ui->t4Z,2,1);
    ui->gridLayout_5->addWidget(&view[T4Z],2,2);

    ui->gridLayout_9->addWidget(ui->t5,1,0);
    ui->gridLayout_9->addWidget(ui->t5X,0,1);
    ui->gridLayout_9->addWidget(&view[T5X],0,2);
    ui->gridLayout_9->addWidget(ui->t5Y,1,1);
    ui->gridLayout_9->addWidget(&view[T5Y],1,2);
    ui->gridLayout_9->addWidget(ui->t5Z,2,1);
    ui->gridLayout_9->addWidget(&view[T5Z],2,2);

    ui->gridLayout_7->addWidget(ui->t6,1,0);
    ui->gridLayout_7->addWidget(ui->t6X,0,1);
    ui->gridLayout_7->addWidget(&view[T6X],0,2);
    ui->gridLayout_7->addWidget(ui->t6Y,1,1);
    ui->gridLayout_7->addWidget(&view[T6Y],1,2);
    ui->gridLayout_7->addWidget(ui->t6Z,2,1);
    ui->gridLayout_7->addWidget(&view[T6Z],2,2);

    ui->gridLayout_13->addWidget(ui->t7,1,0);
    ui->gridLayout_13->addWidget(ui->t7X,0,1);
    ui->gridLayout_13->addWidget(&view[T7X],0,2);
    ui->gridLayout_13->addWidget(ui->t7Y,1,1);
    ui->gridLayout_13->addWidget(&view[T7Y],1,2);
    ui->gridLayout_13->addWidget(ui->t7Z,2,1);
    ui->gridLayout_13->addWidget(&view[T7Z],2,2);

    ui->gridLayout_15->addWidget(ui->t8,1,0);
    ui->gridLayout_15->addWidget(ui->t8X,0,1);
    ui->gridLayout_15->addWidget(&view[T8X],0,2);
    ui->gridLayout_15->addWidget(ui->t8Y,1,1);
    ui->gridLayout_15->addWidget(&view[T8Y],1,2);
    ui->gridLayout_15->addWidget(ui->t8Z,2,1);
    ui->gridLayout_15->addWidget(&view[T8Z],2,2);

    ui->gridLayout_17->addWidget(ui->t9,1,0);
    ui->gridLayout_17->addWidget(ui->t9X,0,1);
    ui->gridLayout_17->addWidget(&view[T9X],0,2);
    ui->gridLayout_17->addWidget(ui->t9Y,1,1);
    ui->gridLayout_17->addWidget(&view[T9Y],1,2);
    ui->gridLayout_17->addWidget(ui->t9Z,2,1);
    ui->gridLayout_17->addWidget(&view[T9Z],2,2);


    ui->gridLayout_17->addWidget(ui->tt1,0,0);
    ui->gridLayout_17->addWidget(ui->tt2,1,0);
    ui->gridLayout_17->addWidget(ui->tt3,2,0);
    ui->gridLayout_17->addWidget(ui->tt4,3,0);
    ui->gridLayout_17->addWidget(ui->tt5,4,0);
    ui->gridLayout_17->addWidget(ui->tt6,5,0);
    ui->gridLayout_17->addWidget(ui->tt7,6,0);
    ui->gridLayout_17->addWidget(ui->tt8,7,0);
    ui->gridLayout_17->addWidget(ui->tt9,8,0);


    ui->gridLayout_11->addWidget(&view2[0],0,1);
    ui->gridLayout_11->addWidget(&view2[1],1,1);
    ui->gridLayout_11->addWidget(&view2[2],2,1);
    ui->gridLayout_11->addWidget(&view2[3],3,1);
    ui->gridLayout_11->addWidget(&view2[4],4,1);
    ui->gridLayout_11->addWidget(&view2[5],5,1);
    ui->gridLayout_11->addWidget(&view2[6],6,1);
    ui->gridLayout_11->addWidget(&view2[7],7,1);
    ui->gridLayout_11->addWidget(&view2[8],8,1);

    ui->stackedWidget->setCurrentIndex(0);
}


//刷新当前数据表
void Widget::refreshCurrentDataTable()
{
    ConnectDataBase *dataBaseConnect = new ConnectDataBase;
    dataBaseConnect->conDataBase();

    sqlModel->setQuery("select * from  "+datatable+" ");
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
    query.exec( "select max(quackTime) from "+datatable+" ;");            // 执行查询操作
    while(query.next()){
        latestDate = query.value(0).toString();
        qDebug()<<"date="<<latestDate;
    }
    ui->lastestIncidentLabel->setText("最新事件："+latestDate);

    dataBaseConnect->close();
    delete dataBaseConnect;
}

//重新选择数据表
//void Widget::reSelectDataBaseSource(QString value)
//{
//    currentDataBase = value;
//    refreshCurrentDataTable();
//}

//控件命令按钮
void Widget::on_axWidget_ImplementCommandEvent(int iCommandId)
{
    if(iCommandId == 1){
        QColor c(255,255,255);
        ui->axWidget->dynamicCall("SetViewColor(QColor)", c);

        QString path = QDir::currentPath();
        qDebug()<<"当前路径"<<path;
        QString tempPath;
        // 调用控件打开dwg文件命令，使用相对路径

        //QString path = QApplication::applicationDirPath();    //是获取的执行文件exe所在的路径
        tempPath=path + "/src/dwg/"+whichRegion;
        ui->axWidget->dynamicCall("OpenDwgFile(const QString&)",tempPath);
        qDebug()<<"tempPath:"<<tempPath;
        tempPath.clear();
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
    ReportForm *reportForm = new ReportForm();
    reportForm->showMaximized();
}


//帮助按钮
void Widget::helpButtonClicked()
{
    Assistant *pAssistant = new Assistant;
    // 按下“帮助”按钮，运行Qt Assistant，显示index.html页面
    pAssistant->showDocumentation("welcome.html");
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
    sqlModel->setQuery("select * from "+datatable);
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
    query.exec( "select max(quackTime) from  "+datatable);            // 执行查询操作
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

    for(int i=0;i<3;i++){
        tempIndex = ui->dataBaseView->model()->index(row,i+2);       //获取CAD定位点
        data = ui->dataBaseView->model()->data(tempIndex);
        coordinates[i] = data.toDouble();
    }
    qDebug()<<"the current coordinate is :"<<coordinates[0]<<" "<<coordinates[1]<<" "<<coordinates[2];

    tempIndex = ui->dataBaseView->model()->index(row,11);            //获取CSV文件路径
    data = ui->dataBaseView->model()->data(tempIndex);
    filePath = data.toString();
    globalStatusBar->showMessage(tr("读取文件：")+filePath);

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

    readCSVFileOfXYZ(filePath);
}

//读取CSV文件数据
bool Widget::readCSVFileOfXYZ(QString fileName)
{
    if(fileName.isEmpty()||fileName.split('/').size() == 0){
        qDebug()<<"fileName is empty or filepath is wrong";
        QMessageBox::warning(this,tr("警告"),tr("请检查文件路径"),QMessageBox::Yes);
        return false;
    }


    QFile file(fileName);
    qDebug()<<"fileName="<<fileName<<'\n';
    QStringList line;
    QStringList item;
    QString block ;
    int k=0;

    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        //文件打开失败
        qDebug()<<"open file failed!";
        //QMessageBox::warning(this,tr("警告"),tr("此文件读取失败"),QMessageBox::Yes);

        QMessageBox *box = new QMessageBox(QMessageBox::Warning,tr("警告"),tr("此文件读取失败"));
        QTimer::singleShot(3000,box,SLOT(accept())); //也可将accept改为close
        box->exec();//box->show();都可以
        return false;
    }

    //每次绘制曲线之前都需要清空上一次数据，所有台站曲线数据都要清空，因为假如一开始有5个，后来有3个台站，那么数据就会混杂在一起
//    for(int i=0;i<27;i++){
//        splineSeries[i].clear();
//        lineSeries[i].clear();
//    }
//    for(int i=0;i<9;i++){
//        splineSeries2[i].clear();
//        lineSeries2[i].clear();
//    }

    QTextStream stream(&file);
    while (!stream.atEnd())
    {
        block = stream.readAll();
        line = block.split('\t');
    }
    int count = line.size();
    qDebug()<<"COUNT ="<<count;
    //限制读取的行数 最多为90000行，如果超过90000行，则舍去多余数据
    if(count > 90000){
        count = 90000;
    }

    //先获取第一行的内容，然后看看该行中有多少个台站，因为文件路径中的盘符所对应的台站名是有激发的
    //而某些CSV文件中存在不激发但是有数据的台站，所以不能根据路径中的盘符来判断
    QStringList valueOfOneLine = line.at(0).split(",");
    int valueOfOneLineSize = valueOfOneLine.size();
    int sennumInRealCSV = (valueOfOneLineSize-1)/8;

    //D:\data\ConstructionData\3moti\vuw z 2021-01-22 06-11-50`63.csv
    QStringList fileSplits = fileName.split('/');
    QStringList allPanfu = fileSplits[fileSplits.length()-1].split(' ');
    QString panfu = allPanfu[0];
    LocationAlgorithm::PANFU = panfu;
    QString notMotivatedPanfu = allPanfu[1];
    int sennum = panfu.length();

    qDebug()<<"SENNUM="<<sennum<<" sennumInRealCSV="<<sennumInRealCSV;
    qDebug()<<"panfu="<<panfu<<" difference="<<sennumInRealCSV-sennum;
    qDebug()<<"notMotivatedPanfu="<<notMotivatedPanfu;
    if(notMotivatedPanfu.size()>(9-sennumInRealCSV)){
        ui->currentIncidentStation->setText("激发台站："+panfu+" 未激发台站：");
    }else{
        ui->currentIncidentStation->setText("激发台站："+panfu+" 未激发台站："+notMotivatedPanfu);
    }

    //存储每个事件触发台站的X，Y，Z轴的数据
    pointBufferX = new QVector<QPointF>[sennumInRealCSV];
    pointBufferY = new QVector<QPointF>[sennumInRealCSV];
    pointBufferZ = new QVector<QPointF>[sennumInRealCSV];
    //用于绘制P波到时（红线）
    pointBuffer_P = new QVector<QPointF>[sennumInRealCSV];


    //存储每个传感器波形的激发位置，在CSV文件中表现为第H列，P列，X列...以此类推
    motiPos = new int[sennumInRealCSV];

    motiPosName = new QString[sennumInRealCSV];

    QString **senChannelZ ;                  //存储每个事件后三个通道中Z轴的数据
    //存储后三个通道中Z轴的数据，在CSV文件中表现为G列，O列，W列...以此类推
    senChannelZ = new QString*[9000];
    for (int i = 0; i < 9000; i++){
        senChannelZ[i] = new QString[sennumInRealCSV];
      //  qDebug()<<"senChannel[i].size="<<senChannel[i]->size();
    }

    QString **senChannelY ;                  //存储每个事件后三个通道中Z轴的数据
    //存储后三个通道中Z轴的数据，在CSV文件中表现为G列，O列，W列...以此类推
    senChannelY = new QString*[9000];
    for (int i = 0; i < 9000; i++){
        senChannelY[i] = new QString[sennumInRealCSV];
    }

    QString **senChannelX ;                  //存储每个事件后三个通道中Z轴的数据
    //存储后三个通道中Z轴的数据，在CSV文件中表现为G列，O列，W列...以此类推
    senChannelX = new QString*[9000];
    for (int i = 0; i < 9000; i++){
        senChannelX[i] = new QString[sennumInRealCSV];
      //  qDebug()<<"senChannel[i].size="<<senChannel[i]->size();
    }


    QString *senChannelNum ;                //存储事件触发台站名称
    senChannelNum = new QString[sennumInRealCSV];
    QChar ch;
    //panfu是按照到时顺序来的
    for(int i=0;i<sennumInRealCSV;i++){
        if(i<sennum){
            ch = panfu.at(i).toUpper();
            senChannelNum[i] = miningAreaLocation.value(QString(ch));
        }
        else{
            ch = notMotivatedPanfu.at(i).toUpper();
            if(!miningAreaLocation.value(QString(ch)).isNull())
                senChannelNum[i] = miningAreaLocation.value(QString(ch));
            else{
                senChannelNum[i] = miningAreaLocation.value("Undefined");
            }
        }
    }


    //这时候已经把盘符获取到了，同时台站名也知道了，然后就把标签设置成台站名称
    //至少有三个台站，所以前三个一定有内容
    ui->t1->setText(senChannelNum[0]);
    ui->t2->setText(senChannelNum[1]);
    ui->t3->setText(senChannelNum[2]);
    if(sennumInRealCSV == 4){
        ui->t4->setText(senChannelNum[3]);
    }else if(sennumInRealCSV == 5){
        ui->t4->setText(senChannelNum[3]);
        ui->t5->setText(senChannelNum[4]);
    }else if(sennumInRealCSV == 6){
        ui->t4->setText(senChannelNum[3]);
        ui->t5->setText(senChannelNum[4]);
        ui->t6->setText(senChannelNum[5]);
    }else if(sennumInRealCSV == 7){
        ui->t4->setText(senChannelNum[3]);
        ui->t5->setText(senChannelNum[4]);
        ui->t6->setText(senChannelNum[5]);
        ui->t7->setText(senChannelNum[6]);
    }else if(sennumInRealCSV == 8){
        ui->t4->setText(senChannelNum[3]);
        ui->t4->setText(senChannelNum[4]);
        ui->t6->setText(senChannelNum[5]);
        ui->t7->setText(senChannelNum[6]);
        ui->t8->setText(senChannelNum[7]);
    }else if(sennumInRealCSV == 9){
        ui->t4->setText(senChannelNum[3]);
        ui->t5->setText(senChannelNum[4]);
        ui->t6->setText(senChannelNum[5]);
        ui->t7->setText(senChannelNum[6]);
        ui->t8->setText(senChannelNum[7]);
        ui->t9->setText(senChannelNum[8]);
    }

    maxValueOfZChannel = 0;
    maxValueOfYChannel = 0;
    maxValueOfXChannel = 0;
    int maxValuOfXYZChannel[9][3];
    for(int m=0;m<9;m++)
        for(int n=0;n<3;n++)
            maxValuOfXYZChannel[m][n]=40000;

    for(int i=0;i<(count-1);i++){
        item = line.at(i).split(',');
        if(0 == i%10){                                    //采样，只取十分之一的数据
            for(int j=0;j<sennumInRealCSV;j++){
                senChannelZ[k][j] = item.at(6+j*8);       //存储每个事件后三个通道中Z轴的数据
                int tempZ = senChannelZ[k][j].toInt();
                if(tempZ > maxValueOfZChannel){
                    maxValueOfZChannel = tempZ;
                    maxValuOfXYZChannel[j][2] = maxValueOfZChannel;
                }

                senChannelY[k][j] = item.at(5+j*8);       //存储每个事件后三个通道中Y轴的数据
                int tempY = senChannelY[k][j].toInt();
                if(tempY > maxValueOfYChannel){
                    maxValueOfYChannel = tempY;
                    maxValuOfXYZChannel[j][1] = maxValueOfYChannel;
                }

                senChannelX[k][j] = item.at(4+j*8);       //存储每个事件后三个通道中X轴的数据
                int tempX = senChannelX[k][j].toInt();
                if(tempX > maxValueOfXChannel){
                    maxValueOfXChannel = tempX;
                    maxValuOfXYZChannel[j][0] = maxValueOfXChannel;
                }

                if(i == 0) {   //存储每个传感器波形激发位置，每行都一样，所以存第一行就行
                    motiPos[j] = item.at(7+j*8).toInt();
                    motiPosName[j] = item.at(8+j*8);
                }
                //qDebug()<<"senChannelX[i][j]="<<senChannelX[i][j]<<'\n';
            }
            //qDebug()<<"K="<<k;
            k++;
        }
        //qDebug()<<"i="<<i<<'\n';
    }

    for(int i=0;i<9;i++){
        if(i<sennumInRealCSV){
             //qDebug()<<"激发位置："<<motiPos[i]<<" 激发名字："<<motiPosName[i];
             LocationAlgorithm::TEMPMOTIPOS[i] = motiPos[i];
        }else{
            motiPos[i] = 0;
            LocationAlgorithm::TEMPMOTIPOS[i]=0;
        }
    }

    file.close();

    qDebug()<<"maxValueOfXYZChannel"<<maxValueOfZChannel<<" "<<maxValueOfYChannel<<" "<<maxValueOfXChannel;
    qDebug()<<"read csv file successfully!";

    //若maxValueOfXYZChannel存在极端值，取最大值40000
    for(int m=0;m<9;m++)
        for(int n=0;n<3;n++)
            if(maxValuOfXYZChannel[m][n]>40000){
                maxValuOfXYZChannel[m][n]=40000;
            }

//    for(int m=0;m<9;m++)
//        for(int n=0;n<3;n++)
//            qDebug()<<"maxValueOfXYZChannel:"<<m<<" "<<n<<":"<<maxValuOfXYZChannel[m][n];

    for(int i=0;i<27;i++){
        //qDebug()<<-maxValuOfXYZChannel[i/3][i%3]<<" "<<maxValuOfXYZChannel[i/3][i%3];
        axisY[i].setRange(-maxValuOfXYZChannel[i/3][i%3], maxValuOfXYZChannel[i/3][i%3]);   //设置坐标轴范围
    }

    for(int j=0;j<sennumInRealCSV;j++){
        //qDebug()<<"motivated position is "<<motiPos[j];  //打印测试站台及其激发位置
        //因为P波到时是一条垂线，所以只需要添加横坐标相同的几个点就好
        pointBuffer_P[j].append(QPointF(motiPos[j]/10,0));
        pointBuffer_P[j].append(QPointF(motiPos[j]/10,50000));
        pointBuffer_P[j].append(QPointF(motiPos[j]/10,-50000));
    }

    int listIndex = 0; 	                                      //可以理解为横坐标，因为横坐标就是每个事件（也就是CSV文件中的一行数据）
    double parseDataZ;
    double parseDataY;
    double parseDataX;
    for(int j=0;j<count;j++){
        if(0 == j%10){
            for(int i=0;i<sennumInRealCSV;i++){
                parseDataZ = senChannelZ[listIndex][i].toDouble();
                pointBufferZ[i].append(QPointF(listIndex, parseDataZ));

                parseDataX = senChannelX[listIndex][i].toDouble();
                pointBufferX[i].append(QPointF(listIndex, parseDataX));

                parseDataY = senChannelY[listIndex][i].toDouble();
                pointBufferY[i].append(QPointF(listIndex, parseDataY));
            }
            listIndex++;
        }
    }

    delete[] senChannelNum;
    for( int i = 0 ; i < 9000 ; i ++ )
        delete []senChannelZ[i] ;
    delete []senChannelZ;
    for( int i = 0 ; i < 9000 ; i ++ )
        delete []senChannelY[i] ;
    delete []senChannelY;
    for( int i = 0 ; i < 9000 ; i ++ )
        delete []senChannelX[i] ;
    delete []senChannelX;

    globalStatusBar->showMessage("文件读取完毕");

    qDebug()<<"check1";
    //每次绘制曲线之前都需要清空上一次数据，所有台站曲线数据都要清空，因为假如一开始有5个，后来有3个台站，那么数据就会混杂在一起
    for(int i=0;i<27;i++){
        splineSeries[i].clear();
        lineSeries[i].clear();
    }
    for(int i=0;i<9;i++){
        splineSeries2[i].clear();
        lineSeries2[i].clear();
    }
     qDebug()<<"check2";
    for(int i=0;i<9;i++){
        //如果有5个台站，那么只需要替换5个台站的曲线数据
        if(i< sennumInRealCSV){
            int a = i*3;
            int b = i*3+1;
            int c = i*3+2;
            splineSeries[a].replace(pointBufferX[i]);
            splineSeries[b].replace(pointBufferY[i]);
            splineSeries[c].replace(pointBufferZ[i]);

            lineSeries[a].replace(pointBuffer_P[i]);
            lineSeries[b].replace(pointBuffer_P[i]);
            lineSeries[c].replace(pointBuffer_P[i]);

            splineSeries2[i].replace(pointBufferX[i]);
            splineSeries2[i].replace(pointBufferY[i]);
            splineSeries2[i].replace(pointBufferZ[i]);

            lineSeries2[i].replace(pointBuffer_P[i]);
            lineSeries2[i].replace(pointBuffer_P[i]);
            lineSeries2[i].replace(pointBuffer_P[i]);
        }
    }
     qDebug()<<"check3";
    //绘制曲线完毕后，把容器的数据删除，然后删除对象
    for(int i=0;i<sennumInRealCSV;i++){
        pointBufferX[i].clear();
        pointBufferY[i].clear();
        pointBufferZ[i].clear();
        pointBuffer_P[i].clear();
    }
    delete [] pointBufferX;
    delete [] pointBufferY;
    delete [] pointBufferZ;
    delete [] pointBuffer_P;

    return true;
}


//T1~T9按钮，用来显示不同站台曲线图
void Widget::on_T1Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    pageSwitchIndex = 0;
}
void Widget::on_T2Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    pageSwitchIndex = 1;
}
void Widget::on_T3Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    pageSwitchIndex = 2;
}
void Widget::on_T4Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    pageSwitchIndex = 3;
}
void Widget::on_T5Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    pageSwitchIndex = 4;
}
void Widget::on_T6Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
    pageSwitchIndex = 5;
}
void Widget::on_T7Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
    pageSwitchIndex = 6;
}
void Widget::on_T8Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(7);
    pageSwitchIndex = 7;
}
void Widget::on_T9Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(8);
    pageSwitchIndex = 8;
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

//全部按钮，显示T1~T9站台全部曲线图信息
void Widget::fullChartsButtonClicked()
{
    qDebug()<<"pageSwitchIndex = "<<pageSwitchIndex<<'\n';
    ui->stackedWidget->setCurrentIndex(9);
}

//消息提示对话框
void Widget::informationDialog()
{
    QMessageBox *msg = new QMessageBox(this);
    msg->setWindowTitle(tr("提示"));
    msg->setText(tr("点击更新将校正后的定位以及P波位置更新到数据库和数据文件中，点击修改只更新P波位置到数据文件"));
    msg->setIcon(QMessageBox::Information);

    QPushButton *updataAllButton = msg->addButton(tr("更新"), QMessageBox::ActionRole);
    QPushButton *updataCSVButton = msg->addButton(tr("修改"),QMessageBox::ActionRole);
    QPushButton *abortButton = msg->addButton(tr("取消"),QMessageBox::RejectRole);
    msg->exec();

    if (msg->clickedButton() == updataAllButton) {
        updatePSOandPwaveData();
    } else if (msg->clickedButton() == updataCSVButton) {
        if(userInput>0 && userInput<90000){
            saveModifiedPWaveData();
        }
        else QMessageBox::warning(this,"警告","请输入有效值",QStringLiteral("确定"));
    }else {
        qDebug()<<"updata locations and P wave arrival operation is cancelled ";
        return;
    }
}
//将输入的P波到时更新到CSV数据文件中以及将定位算法得到的值更新到数据库中
void Widget::updatePSOandPwaveData()
{
    saveModifiedPWaveData();

    ConnectDataBase *db = new ConnectDataBase;
    db->conDataBase();
    db->updateDataBase(ConnectDataBase::EVENTID,
                       LocationAlgorithm::XRESULT.toDouble(),LocationAlgorithm::YRESULT.toDouble(),
                       LocationAlgorithm::ZRESULT.toDouble(),LocationAlgorithm::TRESULT.toDouble());
//    ConnectDataBase::updateDataBase(ConnectDataBase::SQLTABLE,ConnectDataBase::EVENTID,
//                                    LocationAlgorithm::XRESULT.toDouble(),LocationAlgorithm::YRESULT.toDouble(),
//                                    LocationAlgorithm::ZRESULT.toDouble(),LocationAlgorithm::TRESULT.toDouble());
    db->close();
    delete db;
}
//将输入的的P波到时位置保存/更新到CSV数据文件中
void Widget::saveModifiedPWaveData()
{
    QString value =QString::number(userInput);
    QString station = motiPosName[ui->comboBox->currentIndex()];
    QString motiPositon = QString::number(motiPos[ui->comboBox->currentIndex()]) ;
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly)){
        qDebug() << "Cannot open file for Reading";
        return ;
    }
    QTextStream in(&file);
    QString block = in.readAll();
    file.close();

    if(!file.open(QIODevice::WriteOnly)){
        qDebug() << "Cannot open file for Writing";
        return;
    }

    QString newStr = value +","+ station;
    QString oldStr =motiPositon +","+ station;
    qDebug()<<"newStr ="<<newStr<<" "<<"oldStr = "<<oldStr;
    if(block.contains(oldStr, Qt::CaseInsensitive))
    {
        //block.replace(QRegExp("USER=.*"),QString("wor"));
        block.replace(QString(oldStr),QString(newStr));
        file.write(block.toUtf8());
        file.close();

       // globalStatusBar->showMessage(tr("数据")+newStr+tr(" 已更新到：")+ReadCSVData::FILEPATH);
        qDebug()<<"writed successfully!";
    }else {
        file.write(block.toUtf8());
        file.close();
    }
}

//调用定位算法
void Widget::getLoactionData()
{
    globalStatusBar->showMessage(tr("正在计算数据，请等待..."));
    if(userInput>0 && userInput<90000){         //输入数据不超范围时才做处理
        LocationAlgorithm *psoAlgorithm = new LocationAlgorithm();
        connect(psoAlgorithm, &LocationAlgorithm::finished, psoAlgorithm, &QObject::deleteLater);
        psoAlgorithm->start();
    }
}

//重新绘制用户调整P波激发位置后的P波红线
void Widget::repaintPWave()
{
    userInput = ui->intputPWave->text().toInt();
    int stationIndex = ui->comboBox->currentIndex();
    //在这里记录一下用户调整的P波红线，后面重定位算法用的到
    LocationAlgorithm::TEMPMOTIPOS[stationIndex]=userInput;

    //删除台站X、Y、Z方向的旧P波红线
    lineSeries[stationIndex*3].clear();
    lineSeries[stationIndex*3+1].clear();
    lineSeries[stationIndex*3+2].clear();

    lineSeries2[stationIndex].clear();
    lineSeries2[stationIndex].clear();
    lineSeries2[stationIndex].clear();

    //绘制调整后的新P波红线
    lineSeries[stationIndex*3] << QPointF(userInput/10, 0)<<QPointF(userInput/10, 50000)<<QPointF(userInput/10, -50000);
    lineSeries[stationIndex*3+1] << QPointF(userInput/10, 0)<<QPointF(userInput/10, 50000)<<QPointF(userInput/10, -50000);
    lineSeries[stationIndex*3+2] << QPointF(userInput/10, 0)<<QPointF(userInput/10, 50000)<<QPointF(userInput/10, -50000);

    lineSeries2[stationIndex] << QPointF(userInput/10, 0)<<QPointF(userInput/10, 50000)<<QPointF(userInput/10, -50000);
    lineSeries2[stationIndex] << QPointF(userInput/10, 0)<<QPointF(userInput/10, 50000)<<QPointF(userInput/10, -50000);
    lineSeries2[stationIndex] << QPointF(userInput/10, 0)<<QPointF(userInput/10, 50000)<<QPointF(userInput/10, -50000);

}

//获取当前台站的激发位置
void Widget::getCurrentMotipos(int value)
{
    QString motiPositon = QString::number(motiPos[value]) ;
    ui->locationResultLabel->setText(motiPositon);
}


//鼠标悬停到波形图上显示该出坐标点
void Widget::slotPointHoverd(const QPointF &point, bool state)
{
    if (state) {
        m_valueLabel->setText(QString::asprintf("%1.0f,%1.0f", point.x()*10,point.y()));
        QPoint curPos = mapFromGlobal(QCursor::pos());
        m_valueLabel->move(int(curPos.x() - m_valueLabel->width() / 2), int(curPos.y() - m_valueLabel->height() * 1.5));  //移动数值
        m_valueLabel->show();//显示出来
    }
    else {
        m_valueLabel->hide();//进行
    }
}

//chartView事件过滤封装
void Widget::charViewEventFilter(QEvent *event,QChart *tempChart)
{
    if(event->type()==QEvent::Wheel){
        //将event强制转换为发生的事件的类型
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        if(wheelEvent->delta()>0)tempChart->zoom(1.2);
        else tempChart->zoom(10.0/11);
    }
    else if(event->type()==QEvent::MouseButtonPress){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if(mouseEvent->button() & Qt::RightButton)
            tempChart->zoomReset();
    }
    else if(event->type()==QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        switch (keyEvent->key()) {
        case Qt::Key_Left:
            tempChart->scroll(-20, 0);
            break;
        case Qt::Key_Right:
            tempChart->scroll(30, 0);
            break;
        case Qt::Key_Up:
            tempChart->scroll(0, 30);
            break;
        case Qt::Key_Down:
            tempChart->scroll(0, -20);
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
    return QWidget::eventFilter(obj,event);
}
