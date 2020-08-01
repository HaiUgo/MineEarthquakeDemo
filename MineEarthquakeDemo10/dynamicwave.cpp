#include "dynamicwave.h"
#include "ui_dynamicwave.h"

DynamicWave::DynamicWave(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DynamicWave)
{
    ui->setupUi(this);

    axis_x_counts = 0;
    listIndex = 0;
    queueSize = 0;
    isStoppingTimer = false;

    initDynamicCharts();

    connect(ui->showDynWaveButton,SIGNAL(clicked()),this,SLOT(showDynWave()));
    connect(ui->stopDynWaveButton,SIGNAL(clicked()),this,SLOT(stopDynWave()));

    for(int i=0;i<27;i++){
        connect(&splineSeries[i], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
        connect(&lineSeries[i], &QLineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
    }

}

DynamicWave::~DynamicWave()
{
    delete readData;
    delete pointBufferTemp;
    delete splineSeries;
    delete axisX;
    delete axisY;
    delete chart;
    delete view;

    delete ui;
}

void DynamicWave::slotPointHoverd(const QPointF &point, bool state)
{
    if (state) {
        m_valueLabel->raise();
        m_valueLabel->setWindowFlag(Qt::WindowStaysOnTopHint);
        m_valueLabel->setText(QString::asprintf("%1.0f,%1.0f", point.x()*100,point.y()*40));   //横坐标*100，纵坐标*40，将数据还原
        QPoint curPos = mapFromGlobal(QCursor::pos());
        m_valueLabel->move(int(curPos.x() - m_valueLabel->width() / 2), int(curPos.y() - m_valueLabel->height() * 1.5));  //移动数值
        m_valueLabel->show();//显示出来
    }
    else {
        m_valueLabel->hide();//进行
    }
}

//接收widget发来的信号，从而获取相应CSV文件路径
void DynamicWave::receiveCSVFilePath2(QString path)
{
    for(int i=0;i<27;i++){
        splineSeries[i].clear();
        lineSeries[i].clear();
    }

    readData = new ReadCSVData;
    readData->parseCSVFileName(path);
    readData->readCSVFile(path);
    readData->locateCSVData();
}
//初始化图表
void DynamicWave::initDynamicCharts()
{
    splineSeries = new QSplineSeries[27];
    lineSeries = new QLineSeries[27];
    chart = new Chart[27];
    axisX = new QValueAxis[27];
    axisY = new QValueAxis[27];
    view = new ChartView [27];

    m_valueLabel = new QLabel(this);
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::red);
    m_valueLabel->setPalette(pa);

    QPen pen;
    pen.setWidthF(0.5);

    QMargins margin(0,0,0,0);                           //设置chart rectangle（图表矩形）的边距
    //QRectF recf(20,5,260,50);                               //显示设置plot area（图表区域）的大小

    for (int i=0;i<27;i++) {
        splineSeries[i].setColor(QColor(Qt::black));
        splineSeries[i].setUseOpenGL(true);
        splineSeries[i].setPointsVisible(true);
        splineSeries[i].setPen(pen);
        lineSeries[i].setColor(QColor(255,0,0));

        chart[i].legend()->hide();
        chart[i].addSeries(&splineSeries[i]);            //为图表添加曲线序列
        chart[i].addSeries(&lineSeries[i]);              //为图标添加折线序列
        chart[i].setMargins(margin);

        axisX[i].setRange(0, 900);                     //设置坐标轴范围
        axisX[i].setTickCount(10);                       //9区域，10个刻度
        axisX[i].setMinorTickCount(1);                   //单位刻度中间再加一个副刻度

        axisY[i].setRange(-500, 500);
        axisY[i].setTickCount(5);

        chart[i].addAxis(&axisX[i], Qt::AlignBottom);      //把坐标轴添加到chart中，第二个参数是设置坐标轴的位置，
        splineSeries[i].attachAxis(&axisX[i]);             //把曲线关联到坐标轴
        lineSeries[i].attachAxis(&axisX[i]);

        chart[i].addAxis(&axisY[i], Qt::AlignLeft);
        splineSeries[i].attachAxis(&axisY[i]);
        lineSeries[i].attachAxis(&axisY[i]);

        view[i].setChart(&chart[i]);
        //view[i].setRubberBand(QChartView::RectangleRubberBand);
        view[i].installEventFilter(this);                  //注册部件事件过滤
    }

    //将标签和view[T]图表添加到stackWidget的GridLayout布局中
    ui->gridLayout->addWidget(&view[0],0,1);
    ui->gridLayout->addWidget(&view[1],0,2);
    ui->gridLayout->addWidget(&view[2],0,3);
    ui->gridLayout->addWidget(&view[3],1,1);
    ui->gridLayout->addWidget(&view[4],1,2);
    ui->gridLayout->addWidget(&view[5],1,3);
    ui->gridLayout->addWidget(&view[6],2,1);
    ui->gridLayout->addWidget(&view[7],2,2);
    ui->gridLayout->addWidget(&view[8],2,3);
    ui->gridLayout->addWidget(&view[9],3,1);
    ui->gridLayout->addWidget(&view[10],3,2);
    ui->gridLayout->addWidget(&view[11],3,3);
    ui->gridLayout->addWidget(&view[12],4,1);
    ui->gridLayout->addWidget(&view[13],4,2);
    ui->gridLayout->addWidget(&view[14],4,3);
    ui->gridLayout->addWidget(&view[15],5,1);
    ui->gridLayout->addWidget(&view[16],5,2);
    ui->gridLayout->addWidget(&view[17],5,3);
    ui->gridLayout->addWidget(&view[18],6,1);
    ui->gridLayout->addWidget(&view[19],6,2);
    ui->gridLayout->addWidget(&view[20],6,3);
    ui->gridLayout->addWidget(&view[21],7,1);
    ui->gridLayout->addWidget(&view[22],7,2);
    ui->gridLayout->addWidget(&view[23],7,3);
    ui->gridLayout->addWidget(&view[24],8,1);
    ui->gridLayout->addWidget(&view[25],8,2);
    ui->gridLayout->addWidget(&view[26],8,3);
}

//停止动态波形
void DynamicWave::stopDynWave()
{
    if (QObject::sender() == ui->stopDynWaveButton) {
        if (!isStoppingTimer) {
            drawTimer->stop();
        } else {
            drawTimer->start();
        }
        isStoppingTimer = !isStoppingTimer;
    }
}

//显示动态波形
void DynamicWave::showDynWave()
{
    queueSize = readData->dynPointBuffer[T1X].size();   //不一定非要写T1X，写其他的也可以，因为队列长度都是一样的
    qDebug()<<"queueSize="<<queueSize;
    pointBufferTemp = new QVector<QPointF>;
    for(int j=0;j<queueSize;j++){
        pointBufferTemp->append(QPointF(listIndex, 0)); //点坐标集合
        listIndex++;
    }
    //提前添加完点，就不用考虑是否是第一次绘制，只用替换现有点的数据
    for(int j = 0;j<27;j++)
        splineSeries[j].replace(*pointBufferTemp);

    drawTimer = new QTimer(this);                       //定时任务
    connect(drawTimer, SIGNAL(timeout()), this, SLOT(drawDynSplineWave()));
    drawTimer->start(25);
    qDebug()<<"start timer";
}

//绘制动态波形图
void DynamicWave::drawDynSplineWave()
{
    double drawPoint = 0;

    if(readData->dynPointBuffer[T1X].size() != 0){
        drawPoint = readData->dynPointBuffer[T1X].dequeue()/40;          //将纵坐标原始数据缩小40倍
        //qDebug()<<"drawPoint = "<<drawPoint<<" T1X"<<'\n';
        splineSeries[T1X].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));
        drawPoint = readData->dynPointBuffer[T1Y].dequeue()/40;
        //qDebug()<<"drawPoint = "<<drawPoint<<" T1Y"<<'\n';
        splineSeries[T1Y].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));
        drawPoint = readData->dynPointBuffer[T1Z].dequeue()/40;
        //qDebug()<<"drawPoint = "<<drawPoint<<" T1Z"<<'\n';
        splineSeries[T1Z].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));

        //这里将显示P波到时延时到出队队列数据1/9之后
        if(axis_x_counts == queueSize/9){
            //将横坐标缩小100倍
            lineSeries[T1X]<<QPointF(readData->tempMotiPos[1][0]/100,500)<<QPointF(readData->tempMotiPos[1][0]/100,-500);
            lineSeries[T1Y]<<QPointF(readData->tempMotiPos[1][0]/100,500)<<QPointF(readData->tempMotiPos[1][0]/100,-500);
            lineSeries[T1Z]<<QPointF(readData->tempMotiPos[1][0]/100,500)<<QPointF(readData->tempMotiPos[1][0]/100,-500);
            //lineSeries[T1X].replace(readData->pointBuffer_P[1]);
            //lineSeries[T1Y].replace(readData->pointBuffer_P[1]);
            //lineSeries[T1Z].replace(readData->pointBuffer_P[1]);
            //qDebug()<<"T1 P波";
        }

        if(readData->dynPointBuffer[T1X].size() == 0){
            axis_x_counts = 0;
            drawTimer->stop();
            qDebug()<<"T1 dequeue over";
        }
    }
    if(readData->dynPointBuffer[T2X].size() != 0){
        drawPoint = readData->dynPointBuffer[T2X].dequeue()/40;          //将纵坐标原始数据缩小40倍
        splineSeries[T2X].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));
        drawPoint = readData->dynPointBuffer[T2Y].dequeue()/40;
        splineSeries[T2Y].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));
        drawPoint = readData->dynPointBuffer[T2Z].dequeue()/40;
        splineSeries[T2Z].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));

        if(axis_x_counts == queueSize/9){
            lineSeries[T2X]<<QPointF(readData->tempMotiPos[2][0]/100,500)<<QPointF(readData->tempMotiPos[2][0]/100,-500);
            lineSeries[T2Y]<<QPointF(readData->tempMotiPos[2][0]/100,500)<<QPointF(readData->tempMotiPos[2][0]/100,-500);
            lineSeries[T2Z]<<QPointF(readData->tempMotiPos[2][0]/100,500)<<QPointF(readData->tempMotiPos[2][0]/100,-500);
        }

        if(readData->dynPointBuffer[T2X].size() == 0){
            axis_x_counts = 0;
            drawTimer->stop();
            qDebug()<<"T2 dequeue over";
        }
    }
    if(readData->dynPointBuffer[T3X].size() != 0){
        drawPoint = readData->dynPointBuffer[T3X].dequeue()/40;          //将纵坐标原始数据缩小40倍
        splineSeries[T3X].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));
        drawPoint = readData->dynPointBuffer[T3Y].dequeue()/40;
        splineSeries[T3Y].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));
        drawPoint = readData->dynPointBuffer[T3Z].dequeue()/40;
        splineSeries[T3Z].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));

        if(axis_x_counts == queueSize/9){
            lineSeries[T3X]<<QPointF(readData->tempMotiPos[3][0]/100,500)<<QPointF(readData->tempMotiPos[3][0]/100,-500);
            lineSeries[T3Y]<<QPointF(readData->tempMotiPos[3][0]/100,500)<<QPointF(readData->tempMotiPos[3][0]/100,-500);
            lineSeries[T3Z]<<QPointF(readData->tempMotiPos[3][0]/100,500)<<QPointF(readData->tempMotiPos[3][0]/100,-500);
        }

        if(readData->dynPointBuffer[T3X].size() == 0){
            axis_x_counts = 0;
            drawTimer->stop();
            qDebug()<<"T3 dequeue over";
        }
    }
    if(readData->dynPointBuffer[T4X].size() != 0){
        drawPoint = readData->dynPointBuffer[T4X].dequeue()/40;          //将纵坐标原始数据缩小40倍
        splineSeries[T4X].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));
        drawPoint = readData->dynPointBuffer[T4Y].dequeue()/40;
        splineSeries[T4Y].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));
        drawPoint = readData->dynPointBuffer[T4Z].dequeue()/40;
        splineSeries[T4Z].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));

        if(axis_x_counts == queueSize/9){
            lineSeries[T4X]<<QPointF(readData->tempMotiPos[4][0]/100,500)<<QPointF(readData->tempMotiPos[4][0]/100,-500);
            lineSeries[T4Y]<<QPointF(readData->tempMotiPos[4][0]/100,500)<<QPointF(readData->tempMotiPos[4][0]/100,-500);
            lineSeries[T4Z]<<QPointF(readData->tempMotiPos[4][0]/100,500)<<QPointF(readData->tempMotiPos[4][0]/100,-500);
        }

        if(readData->dynPointBuffer[T4X].size() == 0){
            axis_x_counts = 0;
            drawTimer->stop();
            qDebug()<<"T4 dequeue over";
        }
    }
    if(readData->dynPointBuffer[T5X].size() != 0){
        drawPoint = readData->dynPointBuffer[T5X].dequeue()/40;          //将纵坐标原始数据缩小40倍
        splineSeries[T5X].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));
        drawPoint = readData->dynPointBuffer[T5Y].dequeue()/40;
        splineSeries[T5Y].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));
        drawPoint = readData->dynPointBuffer[T5Z].dequeue()/40;
        splineSeries[T5Z].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));

        if(axis_x_counts == queueSize/9){
            lineSeries[T5X]<<QPointF(readData->tempMotiPos[5][0]/100,500)<<QPointF(readData->tempMotiPos[5][0]/100,-500);
            lineSeries[T5Y]<<QPointF(readData->tempMotiPos[5][0]/100,500)<<QPointF(readData->tempMotiPos[5][0]/100,-500);
            lineSeries[T5Z]<<QPointF(readData->tempMotiPos[5][0]/100,500)<<QPointF(readData->tempMotiPos[5][0]/100,-500);
        }

        if(readData->dynPointBuffer[T5X].size() == 0){
            axis_x_counts = 0;
            drawTimer->stop();
            qDebug()<<"T5 dequeue over";
        }
    }
    if(readData->dynPointBuffer[T6X].size() != 0){
        drawPoint = readData->dynPointBuffer[T6X].dequeue()/40;          //将纵坐标原始数据缩小40倍
        splineSeries[T6X].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));
        drawPoint = readData->dynPointBuffer[T6Y].dequeue()/40;
        splineSeries[T6Y].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));
        drawPoint = readData->dynPointBuffer[T6Z].dequeue()/40;
        splineSeries[T6Z].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));

        if(axis_x_counts == queueSize/9){
            lineSeries[T6X]<<QPointF(readData->tempMotiPos[6][0]/100,500)<<QPointF(readData->tempMotiPos[6][0]/100,-500);
            lineSeries[T6Y]<<QPointF(readData->tempMotiPos[6][0]/100,500)<<QPointF(readData->tempMotiPos[6][0]/100,-500);
            lineSeries[T6Z]<<QPointF(readData->tempMotiPos[6][0]/100,500)<<QPointF(readData->tempMotiPos[6][0]/100,-500);
        }

        if(readData->dynPointBuffer[T6X].size() == 0){
            axis_x_counts = 0;
            drawTimer->stop();
            qDebug()<<"T6 dequeue over";
        }
    }
    if(readData->dynPointBuffer[T7X].size() != 0){
        drawPoint = readData->dynPointBuffer[T7X].dequeue()/40;          //将纵坐标原始数据缩小40倍
        splineSeries[T7X].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));
        drawPoint = readData->dynPointBuffer[T7Y].dequeue()/40;
        splineSeries[T7Y].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));
        drawPoint = readData->dynPointBuffer[T7Z].dequeue()/40;
        splineSeries[T7Z].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));

        if(axis_x_counts == queueSize/9){
            lineSeries[T7X]<<QPointF(readData->tempMotiPos[7][0]/100,500)<<QPointF(readData->tempMotiPos[7][0]/100,-500);
            lineSeries[T7Y]<<QPointF(readData->tempMotiPos[7][0]/100,500)<<QPointF(readData->tempMotiPos[7][0]/100,-500);
            lineSeries[T7Z]<<QPointF(readData->tempMotiPos[7][0]/100,500)<<QPointF(readData->tempMotiPos[7][0]/100,-500);
        }

        if(readData->dynPointBuffer[T7X].size() == 0){
            axis_x_counts = 0;
            drawTimer->stop();
            qDebug()<<"T7 dequeue over";
        }
    }
    if(readData->dynPointBuffer[T8X].size() != 0){
        drawPoint = readData->dynPointBuffer[T8X].dequeue()/40;          //将纵坐标原始数据缩小40倍
        splineSeries[T8X].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));
        drawPoint = readData->dynPointBuffer[T8Y].dequeue()/40;
        splineSeries[T8Y].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));
        drawPoint = readData->dynPointBuffer[T8Z].dequeue()/40;
        splineSeries[T8Z].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));

        if(axis_x_counts == queueSize/9){
            lineSeries[T8X]<<QPointF(readData->tempMotiPos[8][0]/100,500)<<QPointF(readData->tempMotiPos[8][0]/100,-500);
            lineSeries[T8Y]<<QPointF(readData->tempMotiPos[8][0]/100,500)<<QPointF(readData->tempMotiPos[8][0]/100,-500);
            lineSeries[T8Z]<<QPointF(readData->tempMotiPos[8][0]/100,500)<<QPointF(readData->tempMotiPos[8][0]/100,-500);
        }

        if(readData->dynPointBuffer[T8X].size() == 0){
            axis_x_counts = 0;
            drawTimer->stop();
            qDebug()<<"T8 dequeue over";
        }
    }
    if(readData->dynPointBuffer[T9X].size() != 0){
        drawPoint = readData->dynPointBuffer[T9X].dequeue()/40;          //将纵坐标原始数据缩小40倍
        splineSeries[T9X].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));
        drawPoint = readData->dynPointBuffer[T9Y].dequeue()/40;
        splineSeries[T9Y].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));
        drawPoint = readData->dynPointBuffer[T9Z].dequeue()/40;
        splineSeries[T9Z].replace(axis_x_counts,QPointF(axis_x_counts,drawPoint));

        if(axis_x_counts == queueSize/9){
            lineSeries[T9X]<<QPointF(readData->tempMotiPos[9][0]/100,500)<<QPointF(readData->tempMotiPos[9][0]/100,-500);
            lineSeries[T9Y]<<QPointF(readData->tempMotiPos[9][0]/100,500)<<QPointF(readData->tempMotiPos[9][0]/100,-500);
            lineSeries[T9Z]<<QPointF(readData->tempMotiPos[9][0]/100,500)<<QPointF(readData->tempMotiPos[9][0]/100,-500);
        }

        if(readData->dynPointBuffer[T9X].size() == 0){
            axis_x_counts = 0;
            drawTimer->stop();
            qDebug()<<"T9 dequeue over";
        }
    }
    axis_x_counts ++;
}


void DynamicWave::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
}
//鼠标移动事件
void DynamicWave::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
}

 //鼠标释放事件
void DynamicWave::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
}

//chartView事件过滤封装
void DynamicWave::charViewEventFilter(QEvent *event,QChart *tempChart)
{
    if(event->type()==QEvent::Wheel){
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event); //将event强制转换为发生的事件的类型
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
bool DynamicWave::eventFilter(QObject *obj, QEvent *event)
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
    return 0;
}

