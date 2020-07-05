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

    db = new ConnectDataBase;
    db->conDataBase();

    readData = new ReadCSVData;
    readData->parseCSVFileName(filePath);
    readData->readCSVFile(filePath);
    readData->locateCSVData();

    initCharts();
   // initTButtonCharts();

    for(int i=0;i<27;i++){
        ui->stackedWidget->addWidget(&view[i]); //添加27个图表到stackedWidget组件
    }

   // ui->stackedWidget->addWidget(viewTButton);

    //以下为信号和槽函数
    connect(ui->displayButton,SIGNAL(clicked()),this,SLOT(displayButtonClick()));
    connect(ui->stopButton,SIGNAL(clicked()),this,SLOT(stopButtonClick()));
    connect(ui->fulllScreenButton,SIGNAL(clicked()),this,SLOT(fulllScreenButtonClicked()));

    connect(ui->startButton,SIGNAL(clicked()),this,SLOT(startButtonClicked()));
    connect(ui->dailyStatement,SIGNAL(clicked()),this,SLOT(dailyStatementClicked()));
    connect(ui->monthlyStatement,SIGNAL(clicked()),this,SLOT(monthlyStatementClicked()));
    connect(ui->helpButton,SIGNAL(clicked()),this,SLOT(helpButtonClicked()));

    connect(ui->zoomIn,SIGNAL(clicked()),this,SLOT(zoomInClicked()));
    connect(ui->zoomOut,SIGNAL(clicked()),this,SLOT(zoomOutClicked()));
    connect(ui->restoreButton,SIGNAL(clicked()),this,SLOT(restoreButtonClicked()));
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
        // 调用控件打开函数，打开.dwg文件。
        ui->axWidget->dynamicCall("OpenDwgFile(const QString&)","C:\\Users\\13696\\Desktop\\项目参考资料\\test.dwg");
    }
    if(iCommandId == 5){
        ui->axWidget->dynamicCall("ZoomScale(double dScale)",0.8);
    }
    if(iCommandId == 6){
        ui->axWidget->dynamicCall("ZoomScale(double dScale)",1.1);
    }
    if(iCommandId == 7){
        ui->axWidget->dynamicCall("ZommAll()");
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

//void Widget::initTButtonCharts()
//{
//    chartTButton = new QChart;
//    viewTButton = new QChartView;
//    chartTButton->addSeries(&splineSeries[0]);
//    chartTButton->addSeries(&splineSeries[1]);
//    chartTButton->addSeries(&splineSeries[2]);


//    viewTButton->setChart(chartTButton);
//    viewTButton->setRubberBand(QChartView::RectangleRubberBand);
//    viewTButton->installEventFilter(this);                //注册部件事件过滤
//}

//显示按钮，启动定时器
void Widget::displayButtonClick()
{
    drawTimer = new QTimer(this);                       //定时任务
//   connect(drawTimer, SIGNAL(timeout()), this, SLOT(timeOutEvent()));
//    drawTimer->start(10);                                //10ms执行一次
//    timeOutEvent();

    drawSplineWave(0, 0);

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
//全屏按钮，显示T1~T9站台全部曲线图信息
void Widget::fulllScreenButtonClicked()
{

}

//定时器每计时到8ms，将引起超时事件，从而绘制一条曲线和一个点序列
void Widget::timeOutEvent()
{
//    double parseDataZ;
//    double tempData;
//    qDebug()<<"timeout:"<<'\n';
//    if (originListIndex >= 90000)
//    {
//        drawTimer->stop();
//        qDebug()<<"stop"<<'\n';
//    }
//    else {
//        qDebug()<<"readData->count="<<readData->count<<'\n';
//        qDebug()<<"readData->senNum="<<readData->senNum<<'\n';
//        for(int i = 0 ;i<readData->count;i++){
//            for(int j=0;j<readData->senNum ; j++){
//                parseDataZ = readData->senChannelZ[i][j].toDouble();
//                qDebug() << QString("数据为：%1").arg(parseDataZ);
//                tempData = parseDataZ/10000;
//                pointBuffer.append(QPointF(originListIndex, tempData)); //点坐标集合
//            }
//           originListIndex++;
//        }
//    }
}

//绘制曲线图
void Widget::drawSplineWave(int axis_x, double data)
{
    //pointBuffer.append(QPointF(axis_x, data)); //点坐标集合
    //splineSeries->replace(pointBuffer);
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
    if (isClickingChart) {
        xOld = yOld = 0;
        isClickingChart = false;
    }
}


//T1按钮，显示T1站台曲线图
void Widget::on_T1Button_clicked()
{
//    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
//    vboxLayout->addWidget(&view[T1X]);
//    vboxLayout->addWidget(&view[T1Y]);
//    vboxLayout->addWidget(&view[T1Z]);
//    QWidget   *chartViewWidget = new QWidget(this);
//    chartViewWidget->setLayout(vboxLayout);
//    ui->stackedWidget->addWidget(chartViewWidget);
//    ui->stackedWidget->setCurrentWidget(chartViewWidget);
//    ui->stackedWidget->setLayout(vboxLayout);
//    ui->stackedWidget->setCurrentWidget(viewTButton);
}
void Widget::on_t1X_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T1X]);
}
void Widget::on_t1Y_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T1Y]);
}
void Widget::on_t1Z_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T1Z]);
}
//T2按钮，显示T2站台曲线图
void Widget::on_T2Button_clicked()
{

}
void Widget::on_t2X_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T2X]);
}
void Widget::on_t2Y_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T2Y]);
}
void Widget::on_t2Z_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T2Z]);
}
//T3按钮，显示T3站台曲线图
void Widget::on_T3Button_clicked()
{

}
void Widget::on_t3X_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T3X]);
}
void Widget::on_t3Y_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T3Y]);
}
void Widget::on_t3Z_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T3Z]);
}
//T4按钮，显示T4站台曲线图
void Widget::on_T4Button_clicked()
{

}
void Widget::on_t4X_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T4X]);
}
void Widget::on_t4Y_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T4Y]);
}
void Widget::on_t4Z_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T4Z]);
}
//T5按钮，显示T5站台曲线图
void Widget::on_T5Button_clicked()
{

}
void Widget::on_t5X_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T5X]);
}
void Widget::on_t5Y_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T5Y]);
}
void Widget::on_t5Z_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T5Z]);
}
//T6按钮，显示T6站台曲线图
void Widget::on_T6Button_clicked()
{

}
void Widget::on_t6X_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T6X]);
}
void Widget::on_t6Y_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T6Y]);
}
void Widget::on_t6Z_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T6Z]);
}
//T7按钮，显示T7站台曲线图
void Widget::on_T7Button_clicked()
{

}
void Widget::on_t7X_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T7X]);
}
void Widget::on_t7Y_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T7Y]);
}
void Widget::on_t7Z_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T7Z]);
}
//T8按钮，显示T8站台曲线图
void Widget::on_T8Button_clicked()
{

}
void Widget::on_t8X_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T8X]);
}
void Widget::on_t8Y_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T8Y]);
}
void Widget::on_t8Z_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T8Z]);
}
//T9按钮，显示T9站台曲线图
void Widget::on_T9Button_clicked()
{

}
void Widget::on_t9X_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T9X]);
}
void Widget::on_t9Y_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T9Y]);
}
void Widget::on_t9Z_clicked()
{
    ui->stackedWidget->setCurrentWidget(&view[T9Z]);
}


