#include "drawthread.h"

DrawThread::DrawThread(QObject *parent):
    QObject(parent)
{

}

DrawThread::~DrawThread()
{
    qDebug()<<"DrawThread::~DrawThread()";
}

void DrawThread::doDrawSplineWork()
{
    readData = ReadCSVData::getInstance();
    qDebug()<<"DrawThread :ReadCSVData::getInstance() = "<<readData;

    for(int i=0;i<27;i++){
        showChart->splineSeries2[i].clear();
        showChart->lineSeries2[i].clear();
        showChart->splineSeries[i].clear();
        showChart->lineSeries[i].clear();
    }

    for(int i=0;i<27;i++){
        showChart->splineSeries2[i].replace(readData->pointBuffer[i]);
    }
    for(int i=0;i<27;i++){
        showChart->splineSeries[i].replace(readData->pointBuffer[i]);
    }

    //根据TEMPMOTIPOS[j][0]获取站台及激发位置，然后将其绘制到图表中（P波到时）
    for(int j=0;j<10;j++){
        if(ReadCSVData::TEMPMOTIPOS[j][0]!=0){
            if(1==j){                                 //如果站台是1站台，则将P波到时绘制到T1的XYZ轴曲线图表中
                showChart->lineSeries[T1X].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries[T1Y].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries[T1Z].replace(readData->pointBuffer_P[j]);

                showChart->lineSeries2[T1X].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries2[T1Y].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries2[T1Z].replace(readData->pointBuffer_P[j]);
            }
            if(2==j){                                 //如果站台是2站台，则将P波到时绘制到T2的XYZ轴曲线图表中，以此类推
                showChart->lineSeries[T2X].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries[T2Y].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries[T2Z].replace(readData->pointBuffer_P[j]);

                showChart->lineSeries2[T2X].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries2[T2Y].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries2[T2Z].replace(readData->pointBuffer_P[j]);
            }
            if(3==j){
                showChart->lineSeries[T3X].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries[T3Y].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries[T3Z].replace(readData->pointBuffer_P[j]);

                showChart->lineSeries2[T3X].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries2[T3Y].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries2[T3Z].replace(readData->pointBuffer_P[j]);
            }
            if(4==j){
                showChart->lineSeries[T4X].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries[T4Y].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries[T4Z].replace(readData->pointBuffer_P[j]);

                showChart->lineSeries2[T4X].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries2[T4Y].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries2[T4Z].replace(readData->pointBuffer_P[j]);
            }
            if(5==j){
                showChart->lineSeries[T5X].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries[T5Y].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries[T5Z].replace(readData->pointBuffer_P[j]);

                showChart->lineSeries2[T5X].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries2[T5Y].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries2[T5Z].replace(readData->pointBuffer_P[j]);
            }
            if(6==j){
                showChart->lineSeries[T6X].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries[T6Y].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries[T6Z].replace(readData->pointBuffer_P[j]);

                showChart->lineSeries2[T6X].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries2[T6Y].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries2[T6Z].replace(readData->pointBuffer_P[j]);
            }
            if(7==j){
                showChart->lineSeries[T7X].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries[T7Y].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries[T7Z].replace(readData->pointBuffer_P[j]);

                showChart->lineSeries2[T7X].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries2[T7Y].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries2[T7Z].replace(readData->pointBuffer_P[j]);
            }
            if(8==j){
                showChart->lineSeries[T8X].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries[T8Y].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries[T8Z].replace(readData->pointBuffer_P[j]);

                showChart->lineSeries2[T8X].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries2[T8Y].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries2[T8Z].replace(readData->pointBuffer_P[j]);
            }
            if(9==j){
                showChart->lineSeries[T9X].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries[T9Y].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries[T9Z].replace(readData->pointBuffer_P[j]);

                showChart->lineSeries2[T9X].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries2[T9Y].replace(readData->pointBuffer_P[j]);
                showChart->lineSeries2[T9Z].replace(readData->pointBuffer_P[j]);
            }
        }
    }
    qDebug()<<"ThreadID:"<<QThread::currentThreadId();

    QThread::msleep(1500);                             //睡眠1500ms

    //释放内存空间
    delete[] readData->pointBuffer;
    delete[] readData->pointBuffer_P;
}
