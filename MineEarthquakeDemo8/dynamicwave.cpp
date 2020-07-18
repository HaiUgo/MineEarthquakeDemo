#include "dynamicwave.h"
#include "ui_dynamicwave.h"

DynamicWave::DynamicWave(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DynamicWave)
{
    ui->setupUi(this);

    filePath = "C:/Users/13696/Desktop/项目参考资料/wave/5moti/yzwu 2020-06-18 04-19-21`28.csv";
    //filePath = "D:/MicroquakeSystem_LiaoningUniversity/MicroquakeSystem_LiaoningUniversity/data/ConstructionData/3moti/suz 2020-06-15 09-17-48`07.csv";
    originListIndex = 0;         //一定要初始化，否则可能产生随机数
    isStoppingTimer = false;

    pageSwitchIndex = 0;

    initDynamicCharts();

    connect(ui->showDynWaveButton,&QPushButton::clicked,this,&DynamicWave::showDynWave);
    connect(ui->stopDynWaveButton,&QPushButton::clicked,this,&DynamicWave::stopDynWave);

//    connect(&splineSeries[0], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[1], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[2], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[3], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[4], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[5], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[6], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[7], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[8], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[9], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[10], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[11], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[12], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[13], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[14], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[15], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[16], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[17], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[18], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[19], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[20], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[21], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[22], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[23], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[24], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[25], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
//    connect(&splineSeries[26], &QSplineSeries::hovered, this, &DynamicWave::slotPointHoverd);//用于鼠标移动到点上显示数值
}

DynamicWave::~DynamicWave()
{
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


void DynamicWave::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    //|窗口关闭之前需要的操作~
    emit closeDynWaveWindow();
}

//初始化图表
void DynamicWave::initDynamicCharts()
{
    splineSeries = new QSplineSeries[27];
    lineSeries = new QLineSeries[27];
    scatterSeries = new QScatterSeries[27];
    chart = new Chart[27];
    axisX = new QValueAxis[27];
    axisY = new QValueAxis[27];
    view = new ChartView [27];

    m_valueLabel = new QLabel(this);

    QMargins margin(0,0,0,0);                           //设置chart rectangle（图表矩形）的边距
    //QRectF recf(20,5,260,50);                               //显示设置plot area（图表区域）的大小

    for (int i=0;i<27;i++) {
        splineSeries[i].setColor(QColor(Qt::black));
        splineSeries[i].setUseOpenGL(true);
        splineSeries[i].setPointsVisible(true);
        scatterSeries[i].setMarkerShape(QScatterSeries::MarkerShapeCircle);//圆形的点
        scatterSeries[i].setMarkerSize(2);
        lineSeries[i].setColor(QColor(255,0,0));

        chart[i].legend()->hide();
        chart[i].addSeries(&splineSeries[i]);            //为图表添加曲线序列
        chart[i].addSeries(&lineSeries[i]);              //为图标添加折线序列
        chart[i].addSeries(&scatterSeries[i]);           //为图表添加点序列
        chart[i].setMargins(margin);

        axisX[i].setRange(0, 900);                     //设置坐标轴范围
        axisX[i].setTickCount(10);                       //9区域，10个刻度
        axisX[i].setMinorTickCount(1);                   //单位刻度中间再加一个副刻度

        axisY[i].setRange(-50, 50);
        axisY[i].setTickCount(5);

        chart[i].addAxis(&axisX[i], Qt::AlignBottom);      //把坐标轴添加到chart中，第二个参数是设置坐标轴的位置，
        splineSeries[i].attachAxis(&axisX[i]);             //把曲线关联到坐标轴
        scatterSeries[i].attachAxis(&axisX[i]);
        lineSeries[i].attachAxis(&axisX[i]);

        chart[i].addAxis(&axisY[i], Qt::AlignLeft);
        splineSeries[i].attachAxis(&axisY[i]);
        lineSeries[i].attachAxis(&axisY[i]);
        scatterSeries[i].attachAxis(&axisY[i]);

        view[i].setChart(&chart[i]);
        view[i].setRubberBand(QChartView::RectangleRubberBand);
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

//显示动态波形
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

//停止动态波形
void DynamicWave::showDynWave()
{
    drawTimer = new QTimer(this);                       //定时任务
    connect(drawTimer, SIGNAL(timeout()), this, SLOT(drawDynSplineWave()));
    drawTimer->start(10);
}

 //鼠标移动事件
void DynamicWave::mouseMoveEvent(QMouseEvent *event)
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
void DynamicWave::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    if (isClickingChart) {
        xOld = yOld = 0;
        isClickingChart = false;
    }
}
