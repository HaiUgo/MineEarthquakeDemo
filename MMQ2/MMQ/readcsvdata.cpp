#include "readcsvdata.h"

ReadCSVData::ReadCSVData(QObject *parent):QThread(parent)
{
    stopped = false;
    fileReadDone = false;
    maxValueOfXChannel = 0;
    maxValueOfYChannel = 0;
    maxValueOfZChannel = 0;
}

ReadCSVData::~ReadCSVData()
{
    qDebug()<<"ReadCSVData::~ReadCSVData()";
}

//初始化
QString ReadCSVData::FILEPATH = "";

QString ReadCSVData::PANFU = "";

int ReadCSVData::COUNT = 0;

int ReadCSVData::SENNUM = 0;

int ReadCSVData::TEMPMOTIPOS[10] = {0};

int ReadCSVData::TEMPSTATION[10] = {0};

bool ReadCSVData::READFILEFINISHED = false;

int ReadCSVData::maxValueOfXYZChannel = 1000;

//存储每个事件触发台站的X，Y，Z轴的数据
QVector<QPointF> *ReadCSVData::POINTBUFFER = new QVector<QPointF>[27];

//用于绘制P波到时（红线）
QVector<QPointF> *ReadCSVData::POINTBUFFER_P = new QVector<QPointF>[10];

//该函数功能为解析文件路径，动态生成存储数组
void ReadCSVData::parseCSVFileName(QString filePath)
{
    QStringList fileSplits = filePath.split('/');
    PANFU = fileSplits[fileSplits.length()-1].split(' ')[0];
    SENNUM = PANFU.length();
    qDebug()<<"SENNUM="<<SENNUM;
    //存储每个传感器波形的激发位置，在CSV文件中表现为第H列，P列，X列...以此类推
    motiPos = new int[SENNUM];

    //存储每个事件触发日期，在CSV文件中表现为每行的第一列，即日期
    //date = new QString[(STARTTIME+ENDTIME)*5000];

    //存储后三个通道中Z轴的数据，在CSV文件中表现为G列，O列，W列...以此类推
    senChannelZ = new QString*[(STARTTIME+ENDTIME)*(FREQUENCY+200)/10];
    for (int i = 0; i < ((STARTTIME+ENDTIME)*5000)/10; i++){
        senChannelZ[i] = new QString[SENNUM];
      //  qDebug()<<"senChannel[i].size="<<senChannel[i]->size();
    }

    //存储后三个通道中Y轴的数据，在CSV文件中表现为F列，N列，V列...以此类推
    senChannelY = new QString*[(STARTTIME+ENDTIME)*(FREQUENCY+200)/10];
    for (int i = 0; i < ((STARTTIME+ENDTIME)*5000)/10; i++){
        senChannelY[i] = new QString[SENNUM];
      //  qDebug()<<"senChannel[i].size="<<senChannel[i]->size();
    }

    //存储后三个通道中X轴的数据，在CSV文件中表现为E列，M列，U列...以此类推
    senChannelX = new QString*[(STARTTIME+ENDTIME)*(FREQUENCY+200)/10];
    for (int i = 0; i < ((STARTTIME+ENDTIME)*5000)/10; i++){
        senChannelX[i] = new QString[SENNUM];
      //  qDebug()<<"senChannel[i].size="<<senChannel[i]->size();
    }

    //存储每个事件触发的台站名称，在CSV文件中表现为I列，Q列，Y列...以此类推
    senChannelNum = new QString*[(STARTTIME+ENDTIME)*(FREQUENCY+200)/10];
    for (int i = 0; i < ((STARTTIME+ENDTIME)*5000)/10; i++){
        senChannelNum[i] = new QString[SENNUM];
      //  qDebug()<<"senChannelNum[i].size="<<senChannelNum[i]->size();
    }

    qDebug()<<"motiPos:"<<motiPos;
    //qDebug()<<"date:"<<date;
    qDebug()<<"senChannelZ:"<<senChannelZ;
    qDebug()<<"senChannelY:"<<senChannelY;
    qDebug()<<"senChannelX:"<<senChannelX;
    qDebug()<<"senChannelNum:"<<senChannelNum;
    qDebug()<<"pointBuffer:"<<POINTBUFFER;
    qDebug()<<"pointBuffer_P:"<<POINTBUFFER_P;
    qDebug()<<"parsing the csv file completed";
}


//读取CSV文件数据
void ReadCSVData::readCSVFile(QString fileName)
{
    QDir dir = QDir::current();
    QFile file(dir.filePath(fileName));
    qDebug()<<"fileName="<<fileName<<'\n';
    QStringList line;
    QStringList item;
    QString block ;
    int k=0;

    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        //文件打开失败，将标志置为true
        StatusbarMonitoring::isOpenFileError = true;
        qDebug()<<"open file failed!";
        return ;
    }
    QTextStream stream(&file);

    while (!stream.atEnd())
    {
        block = stream.readAll();
        line = block.split('\t');
    }
    COUNT = line.size();
    qDebug()<<"COUNT ="<<COUNT;

    //限制读取的行数 最多为90000行，如果超过90000行，则舍去多余数据
    if(COUNT > 90000){
        COUNT = 90000;
    }

    for(int i=0;i<(COUNT-1);i++){
        item = line.at(i).split(',');
        //date[i] = item.at(0);                         //存储每个事件的日期
        if(0 == i%10){                                    //采样，只取十分之一的数据
            for(int j=0;j<SENNUM;j++){
                senChannelZ[k][j] = item.at(6+j*8);       //存储每个事件后三个通道中Z轴的数据
                int tempZ = senChannelZ[k][j].toInt();
                if(tempZ > maxValueOfZChannel)
                    maxValueOfZChannel = tempZ;

                senChannelY[k][j] = item.at(5+j*8);       //存储每个事件后三个通道中Y轴的数据
                int tempY = senChannelY[k][j].toInt();
                if(tempY > maxValueOfYChannel)
                    maxValueOfYChannel = tempY;

                senChannelX[k][j] = item.at(4+j*8);       //存储每个事件后三个通道中X轴的数据
                int tempX = senChannelX[k][j].toInt();
                if(tempX > maxValueOfXChannel)
                    maxValueOfXChannel = tempX;

                senChannelNum[k][j] = item.at(8+j*8);     //存储每个事件触发台站名称
                motiPos[j] = item.at(7+j*8).toInt();      //存储每个传感器波形激发位置
                //qDebug()<<"senChannelX[i][j]="<<senChannelX[i][j]<<'\n';
            }
            //qDebug()<<"K="<<k;
            k++;
        }
        //qDebug()<<"i="<<i<<'\n';
    }
    file.close();
    fileReadDone = true;

    qDebug()<<"maxValueOfXChannel"<<maxValueOfXChannel;
    qDebug()<<"maxValueOfYChannel"<<maxValueOfYChannel;
    qDebug()<<"maxValueOfZChannel"<<maxValueOfZChannel;

    int temp = (maxValueOfXChannel > maxValueOfYChannel)?maxValueOfXChannel:maxValueOfYChannel;
    maxValueOfXYZChannel = (temp > maxValueOfZChannel)?temp:maxValueOfZChannel;

//    if(maxValueOfXYZChannel)
//        emit sendChartsYAxis();

    qDebug()<<"maxValueOfXYZChannel"<<maxValueOfXYZChannel;
    qDebug()<<"read csv file successfully!";
}

void ReadCSVData::locateCSVData()
{
    int tempStation[9]={0};                                        //按照CSV文件中站台顺序存储站台名称，最多有9个站台

    for(int i=0;i<10;i++){
        TEMPMOTIPOS[i]=0;
        TEMPSTATION[i]=0;
    }
    for(int i=0;i<SENNUM;i++){
        tempStation[i] = (senChannelNum[0][i]).toInt();            //获取站台名称并存储
        TEMPSTATION[i] = (senChannelNum[0][i]).toInt();
        qDebug()<<"tempStation[i]="<<tempStation[i];
    }

    for(int i=0;i<SENNUM;i++){
        switch(tempStation[i])                                     //判断是哪一个站台
        {
            //因为CSV文件中台站及其对应XYZ通道的数据的存储位置（表现在CSV文件中的列数据）是不固定的，
            //所以使用前必须要定位到该台站在哪一列，从而才能够定位到该站台X，Y，Z轴以及激发位置在哪一列
            case 1:
                paddingPointBuffer(&POINTBUFFER[P1X],&POINTBUFFER[P1Y],&POINTBUFFER[P1Z],i);
                TEMPMOTIPOS[tempStation[i]] = motiPos[i];
                break;
            case 2:
                paddingPointBuffer(&POINTBUFFER[P2X],&POINTBUFFER[P2Y],&POINTBUFFER[P2Z],i);
                TEMPMOTIPOS[tempStation[i]] = motiPos[i];
                break;
            case 3:
                paddingPointBuffer(&POINTBUFFER[P3X],&POINTBUFFER[P3Y],&POINTBUFFER[P3Z],i);
                TEMPMOTIPOS[tempStation[i]] = motiPos[i];
                break;
            case 4:
                paddingPointBuffer(&POINTBUFFER[P4X],&POINTBUFFER[P4Y],&POINTBUFFER[P4Z],i);
                TEMPMOTIPOS[tempStation[i]] = motiPos[i];
                break;
            case 5:
                paddingPointBuffer(&POINTBUFFER[P5X],&POINTBUFFER[P5Y],&POINTBUFFER[P5Z],i);
                TEMPMOTIPOS[tempStation[i]] = motiPos[i];
                break;
            case 6:
                paddingPointBuffer(&POINTBUFFER[P6X],&POINTBUFFER[P6Y],&POINTBUFFER[P6Z],i);
                TEMPMOTIPOS[tempStation[i]] = motiPos[i];
                break;
            case 7:
                paddingPointBuffer(&POINTBUFFER[P7X],&POINTBUFFER[P7Y],&POINTBUFFER[P7Z],i);
                TEMPMOTIPOS[tempStation[i]] = motiPos[i];
                break;
            case 8:
                paddingPointBuffer(&POINTBUFFER[P8X],&POINTBUFFER[P8Y],&POINTBUFFER[P8Z],i);
                TEMPMOTIPOS[tempStation[i]] = motiPos[i];
                break;
            case 9:
                paddingPointBuffer(&POINTBUFFER[P9X],&POINTBUFFER[P9Y],&POINTBUFFER[P9Z],i);
                TEMPMOTIPOS[tempStation[i]] = motiPos[i];
                break;
        }
    }
    for(int j=0;j<10;j++){
        if(TEMPMOTIPOS[j]!=0){
            qDebug()<<"this station is"<<j<<"and its motivated position is "<<TEMPMOTIPOS[j];  //打印测试站台及其激发位置
            //因为P波到时是一条垂线，所以只需要添加横坐标相同的几个点就好
            POINTBUFFER_P[j].append(QPointF(TEMPMOTIPOS[j]/10,0));
            POINTBUFFER_P[j].append(QPointF(TEMPMOTIPOS[j]/10,50000));
            POINTBUFFER_P[j].append(QPointF(TEMPMOTIPOS[j]/10,-50000));
        }
    }

    //程序运行到这里表示已经读取文件结束并且每个台站都定位到其数据列了，所以可以将READFILEFINISHED置为true
    READFILEFINISHED = true;

    //删除野指针，避免内存泄漏
    delete[] motiPos;
    //delete[] date;
    for( int i = 0 ; i < 90000/10 ; i ++ )
        delete []senChannelZ[i] ;
    delete []senChannelZ;
    for( int i = 0 ; i < 90000/10 ; i ++ )
        delete []senChannelY[i] ;
    delete []senChannelY;
    for( int i = 0 ; i < 90000/10 ; i ++ )
        delete []senChannelX[i] ;
    delete []senChannelX;
    for( int i = 0 ; i < 90000/10 ; i ++ )
        delete []senChannelNum[i] ;
    delete []senChannelNum;

    qDebug()<<"locateCSVData successfully!";
}

//入参传入QVector<QPointF>的实例化对象
void ReadCSVData::paddingPointBuffer(QVector<QPointF> *pointBufferX,QVector<QPointF> *pointBufferY,QVector<QPointF> *pointBufferZ,int index)
{
    int listIndex = 0; 	                                      //可以理解为横坐标，因为横坐标就是每个事件（也就是CSV文件中的一行数据）
    double parseDataX;
    double parseDataY;
    double parseDataZ;
    for(int j=0;j<COUNT;j++){
        if(0 == j%10){
            parseDataX = senChannelX[listIndex][index].toDouble();        //QString转为float型
            //qDebug()<<"parseDataX="<<parseDataX<<'\n';
            parseDataY = senChannelY[listIndex][index].toDouble();
            //qDebug()<<"parseDataY="<<parseDataY<<'\n';
            parseDataZ = senChannelZ[listIndex][index].toDouble();
            //qDebug()<<"parseDataZ="<<parseDataZ<<'\n';
            pointBufferX->append(QPointF(listIndex, parseDataX)); //点坐标添加到缓存中
            pointBufferY->append(QPointF(listIndex, parseDataY));
            pointBufferZ->append(QPointF(listIndex, parseDataZ));

            //qDebug()<<"listIndex= "<<listIndex;
            listIndex++;
        }
    }
    qDebug()<<"paddingPointBuffer successfully!";
}

void ReadCSVData::run()
{
    //QMutexLocker locker(&mutex);
    //mutex.lock();
    for(int i=0;i<27;i++){
        ReadCSVData::POINTBUFFER[i].clear();
    }
    for(int i=0;i<10;i++){
        ReadCSVData::POINTBUFFER_P[i].clear();
    }
    parseCSVFileName(ReadCSVData::FILEPATH);
    readCSVFile(ReadCSVData::FILEPATH);
    if(fileReadDone){
        locateCSVData();
        StatusbarMonitoring::StatusBarFILEPATH = ReadCSVData::FILEPATH;
        StatusbarMonitoring::isStatusBarFILEPATH = true;
    }else{
        StatusbarMonitoring::StatusBarFILEPATH = ReadCSVData::FILEPATH;
    }
    qDebug()<<"ReadCSVData ThreadID:"<<QThread::currentThreadId();
    //msleep(10);
    //mutex.unlock();
}

void ReadCSVData::stop()
{
    stopped = true;
}
