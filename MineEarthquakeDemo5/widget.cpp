#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    filePath = "C:/Users/13696/Desktop/项目参考资料/124637 2020 13-52-55`13.csv";
    originListIndex = 0;
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

    //以下为信号和槽函数
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
        ui->axWidget->dynamicCall("OpenDwgFile(const QString&)","C:\\Users\\13696\\Desktop\\项目参考资料\\红阳三矿.dwg");
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
    ui->tableView_2->setModel(model);
}

//图表初始化
void Widget::initCharts()
{
    splineSeries = new QSplineSeries[27];
    scatterSeries = new QScatterSeries[27];
    chart = new QChart[27];
    axisX = new QValueAxis[27];
    axisY = new QValueAxis[27];
    view = new QChartView[27];
    for (int i=0;i<27;i++) {
        splineSeries[i].setColor(QColor(Qt::black));
        splineSeries[i].setUseOpenGL(true);
        splineSeries[i].setPointsVisible(true);
        scatterSeries[i].setMarkerSize(8);

        chart[i].legend()->hide();
        chart[i].addSeries(&splineSeries[i]);            //为图表添加曲线序列
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

        chart[i].addAxis(&axisY[i], Qt::AlignLeft);
        splineSeries[i].attachAxis(&axisY[i]);
        scatterSeries[i].attachAxis(&axisY[i]);

        view[i].setChart(&chart[i]);
        view[i].setRubberBand(QChartView::RectangleRubberBand);
        view[i].installEventFilter(this);                //注册部件事件过滤
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


//    ui->gridLayout_10->addWidget(&view[T1X],0,0);
//    ui->gridLayout_10->addWidget(&view[T1Y],0,1);
//    ui->gridLayout_10->addWidget(&view[T1Z],0,2);
//    ui->gridLayout_10->addWidget(&view[T2X],1,0);
//    ui->gridLayout_10->addWidget(&view[T2Y],1,1);
//    ui->gridLayout_10->addWidget(&view[T2Z],1,2);
//    ui->gridLayout_10->addWidget(&view[T3X],2,0);
//    ui->gridLayout_10->addWidget(&view[T3Y],2,1);
//    ui->gridLayout_10->addWidget(&view[T3Z],2,2);
//    ui->gridLayout_10->addWidget(&view[T4X],3,0);
//    ui->gridLayout_10->addWidget(&view[T4Y],3,1);
//    ui->gridLayout_10->addWidget(&view[T4Z],3,2);
//    ui->gridLayout_10->addWidget(&view[T5X],4,0);
//    ui->gridLayout_10->addWidget(&view[T5Y],4,1);
//    ui->gridLayout_10->addWidget(&view[T5Z],4,2);
//    ui->gridLayout_10->addWidget(&view[T6X],5,0);
//    ui->gridLayout_10->addWidget(&view[T6Y],5,1);
//    ui->gridLayout_10->addWidget(&view[T6Z],5,2);
//    ui->gridLayout_10->addWidget(&view[T7X],6,0);
//    ui->gridLayout_10->addWidget(&view[T7Y],6,1);
//    ui->gridLayout_10->addWidget(&view[T7Z],6,2);
//    ui->gridLayout_10->addWidget(&view[T8X],7,0);
//    ui->gridLayout_10->addWidget(&view[T8Y],7,1);
//    ui->gridLayout_10->addWidget(&view[T8Z],7,2);
//    ui->gridLayout_10->addWidget(&view[T9X],8,0);
//    ui->gridLayout_10->addWidget(&view[T9Y],8,1);
//    ui->gridLayout_10->addWidget(&view[T9Z],8,2);

    ui->stackedWidget->setCurrentIndex(0);
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
//   connect(drawTimer, SIGNAL(timeout()), this, SLOT(timeOutEvent()));
//    drawTimer->start(10);                                //10ms执行一次
//    timeOutEvent();

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
     ui->stackedWidget->setCurrentIndex(10);
}

//定时器每计时到8ms，将引起超时事件，从而绘制一条曲线和一个点序列
void Widget::timeOutEvent()
{

}

//绘制曲线图
void Widget::drawSplineWave()
{
    //可以改成for循环，但是为了方便程序扩充暂时先逐一添加
    splineSeries[T1X].replace(readData->pointBuffer[P1X]);
    splineSeries[T1Y].replace(readData->pointBuffer[P1Y]);
    splineSeries[T1Z].replace(readData->pointBuffer[P1Z]);
    splineSeries[T2X].replace(readData->pointBuffer[P2X]);
    splineSeries[T2Y].replace(readData->pointBuffer[P2Y]);
    splineSeries[T2Z].replace(readData->pointBuffer[P2Z]);
    splineSeries[T3X].replace(readData->pointBuffer[P3X]);
    splineSeries[T3Y].replace(readData->pointBuffer[P3Y]);
    splineSeries[T3Z].replace(readData->pointBuffer[P3Z]);

    splineSeries[T4X].replace(readData->pointBuffer[P4X]);
    splineSeries[T4Y].replace(readData->pointBuffer[P4Y]);
    splineSeries[T4Z].replace(readData->pointBuffer[P4Z]);
    splineSeries[T5X].replace(readData->pointBuffer[P5X]);
    splineSeries[T5Y].replace(readData->pointBuffer[P5Y]);
    splineSeries[T5Z].replace(readData->pointBuffer[P5Z]);
    splineSeries[T6X].replace(readData->pointBuffer[P6X]);
    splineSeries[T6Y].replace(readData->pointBuffer[P6Y]);
    splineSeries[T6Z].replace(readData->pointBuffer[P6Z]);

    splineSeries[T7X].replace(readData->pointBuffer[P7X]);
    splineSeries[T7Y].replace(readData->pointBuffer[P7Y]);
    splineSeries[T7Z].replace(readData->pointBuffer[P7Z]);
    splineSeries[T8X].replace(readData->pointBuffer[P8X]);
    splineSeries[T8Y].replace(readData->pointBuffer[P8Y]);
    splineSeries[T8Z].replace(readData->pointBuffer[P8Z]);
    splineSeries[T9X].replace(readData->pointBuffer[P9X]);
    splineSeries[T9Y].replace(readData->pointBuffer[P9Y]);
    splineSeries[T9Z].replace(readData->pointBuffer[P9Z]);
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
    if(obj == view){
        charViewEventFilter(event,chart);
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

