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
