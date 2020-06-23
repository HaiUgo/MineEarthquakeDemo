#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    connectMysql();

    originListIndex = 0;
    isStoppingTimer = false;

    view = new QChartView();
    initSplineWaveChart();
    view->setChart(chart);
    view->setRubberBand(QChartView::RectangleRubberBand);

    view2 = new QChartView();
    initSplineWaveChart2();
    view2->setChart(chart2);
    view2->setRubberBand(QChartView::RectangleRubberBand);

    ui->verticalLayout_2->addWidget(view);
    ui->verticalLayout_2->addWidget(view2);

    view->installEventFilter(this);  //注册部件事件过滤
    view2->installEventFilter(this);

    connect(ui->stopButton,SIGNAL(clicked()),this,SLOT(stopButtonClick()));
    connect(ui->displayButton,SIGNAL(clicked()),this,SLOT(displayButtonClick()));
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
    if(!db.open()){
        qDebug()<<"不能连接"<<"connect to mysql error"<<db.lastError().text();
        return ;
    }
    else{
         qDebug()<<"连接成功"<<"connect to mysql OK";
    }
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
void Widget::on_startButton_clicked()
{ 
    ui->axWidget->dynamicCall("DoCommand(const qint32&)",1); //执行控件自定义命令函数，命令的id为2，该值为命令编号，可任取.
    showTable();
    //readCSVFile();
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
}

//在Table Widget上显示CSV文件数据
//int Widget::readCSVFile()
//{
//    //QFileDialog *dialog = new QFileDialog(this);
//   // QString fileName = dialog->getOpenFileName(this,tr("Open File"),"C:/Users/13696/Desktop/项目参考资料",tr("Excel(*.csv)"));
//    QString fileName = "C:/Users/13696/Desktop/项目参考资料/partcsvdata.csv";
//    if(fileName=="")
//    {
//        return -1;
//    }
//    QDir dir = QDir::current();
//    QFile file(dir.filePath(fileName));
//    if(!file.open(QIODevice::ReadOnly))
//    {
//        return -1;
//    }
//    QList<QStringList> data;
//    QStringList row;
//    QString line ;
//    int columnCount = 0;
//    int rowCount = 0;
//    bool isTheFirstLine = true;
//    QTextStream stream(&file);
//    while (!stream.atEnd())
//    {
//        line = stream.readLine();
//        if(line.isEmpty()){
//            break;
//        }
//        for(const QString& cell: line.split(','))
//        {
//            row.append(cell.trimmed());
//            if(isTheFirstLine)
//            {
//                columnCount++;
//                qDebug()<<"columnCount = "<<columnCount<<"\n";
//            }
//        }
//        rowCount++;
//        isTheFirstLine = false;
//        data.append(row);
//    }
//    ui->tableWidget->setRowCount(rowCount);
//    ui->tableWidget->setColumnCount(columnCount);
//    for (int countrow = 0,i=0;countrow < rowCount; countrow++,i++)
//        for(int countcolumn = 0; countcolumn < columnCount; countcolumn++)
//        {
//            ui->tableWidget->setItem(countrow,countcolumn,new QTableWidgetItem(data.at(i)[countcolumn]));
//        }
//    file.close();
// //调试用例
// //    for(int i=0; i<data.size(); ++i)
// //    {
// //        QStringList s1=data.at(i);
// //        QString str=s1.join(" ") + "!";
// //        qDebug()<<str<<"\n";
// //        qDebug()<<"i="<<i<<"\n";
// //    }
//    return 0;
//}

//图表初始化
void Widget::initSplineWaveChart()
{
    splineSeries = new QSplineSeries(); //曲线序列
    splineSeries->setColor(QColor(Qt::black));//设置线的颜色
    splineSeries->setUseOpenGL(true);//openGL加速
    scatterSeries = new QScatterSeries(); //点序列
    scatterSeries->setMarkerSize(8);  //设置描点的大小

    chart = new QChart();
    chart->legend()->hide();    //不显示注释
    //chart->createDefaultAxes();
    chart->setTitle("实时动态曲线");
    chart->addSeries(splineSeries);  //添加曲线到chart中
    chart->addSeries(scatterSeries);

    axisX = new QValueAxis();
    axisX->setRange(0, 90000);  //设置坐标轴范围
    axisX->setTitleText("time(0.2millisecs)");//标题
    axisX->setTickCount(10); //9区域，10个刻度
    axisX->setMinorTickCount(1); //单位刻度中间再加一个副刻度

    axisY = new QValueAxis();
    axisY->setRange(-2, 2);
    axisY->setTitleText("accelerated speed");
    axisY->setTickCount(5);

    chart->addAxis(axisX, Qt::AlignBottom); //把坐标轴添加到chart中，第二个参数是设置坐标轴的位置，
    splineSeries->attachAxis(axisX);  //把曲线关联到坐标轴
    scatterSeries->attachAxis(axisX);

    chart->addAxis(axisY, Qt::AlignLeft);
    splineSeries->attachAxis(axisY);
    scatterSeries->attachAxis(axisY);
}

void Widget::initSplineWaveChart2()
{
    splineSeries2 = new QSplineSeries(); //曲线序列
    splineSeries2->setColor(QColor(Qt::black));//设置线的颜色
    splineSeries2->setUseOpenGL(true);//openGL加速
    scatterSeries2 = new QScatterSeries(); //点序列
    scatterSeries2->setMarkerSize(8);  //设置描点的大小

    chart2 = new QChart();
    chart2->legend()->hide();    //不显示注释
    //chart2->createDefaultAxes();
    chart2->setTitle("实时动态曲线");
    chart2->addSeries(splineSeries2);  //添加曲线到chart中
    chart2->addSeries(scatterSeries2);


    axisX2 = new QValueAxis();
    axisX2->setRange(0, 90000);  //设置坐标轴范围
    axisX2->setTitleText("time(0.2millisecs)");//标题
    axisX2->setTickCount(10); //18个区域，19个刻度
    axisX2->setMinorTickCount(1);

    axisY2 = new QValueAxis();
    axisY2->setRange(-2, 2);
    axisY2->setTitleText("accelerated speed");
    axisY2->setTickCount(5);

    chart2->addAxis(axisX2, Qt::AlignBottom);
    splineSeries2->attachAxis(axisX2);  //把曲线关联到坐标轴
    scatterSeries2->attachAxis(axisX2);
    chart2->addAxis(axisY2, Qt::AlignLeft);
    splineSeries2->attachAxis(axisY2);
    scatterSeries2->attachAxis(axisY2);
}

//显示 按钮
void Widget::displayButtonClick()
{
    QString fileName = "C:/Users/13696/Desktop/项目参考资料/new.csv";
    QDir dir = QDir::current();
    QFile file(dir.filePath(fileName));
    if(!file.open(QIODevice::ReadOnly))
        return ;

    QTextStream stream(&file);
    while (!stream.atEnd())
    {
        origin.push_back(stream.readLine());
    }
    file.close();

    originList = origin.split(",");
    originListSize = originList.count();
    qDebug() << QString("数据大小%1").arg(originListSize);

    drawTimer = new QTimer(this);//定时任务
    connect(drawTimer, SIGNAL(timeout()), this, SLOT(timeOutEvent()));
    drawTimer->start(8);//8ms执行一次
    qDebug()<<"执行"<<'\n';
}

//暂停按钮
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

//定时器每计时到8ms，将引起超时事件，从而绘制一条曲线和一个点序列
void Widget::timeOutEvent()
{
    qDebug()<<"timeout:"<<'\n';
    if (originListIndex >= originListSize)
    {
        drawTimer->stop();
        qDebug()<<"stop"<<'\n';
    }
    else {
        double parseData = originList.at(originListIndex).toDouble();
        qDebug() << QString("数据为：%1").arg(parseData);
        double tempData = parseData/10000;
        drawSplineWave(originListIndex, tempData);
        originListIndex++;
    }
}

//绘制曲线图
void Widget::drawSplineWave(int axis_x, double data)
{
    pointBuffer.append(QPointF(axis_x, data)); //点坐标集合
    splineSeries->replace(pointBuffer);
    splineSeries2->replace(pointBuffer);
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
    if(obj == view2){
        charViewEventFilter(event,chart2);
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


