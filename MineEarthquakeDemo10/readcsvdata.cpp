#include "readcsvdata.h"

ReadCSVData::ReadCSVData(QObject *parent):
    QObject(parent)
{

}
int ReadCSVData::tempMotiPos[10][1] = {{0}};

//删除野指针，避免内存泄漏
ReadCSVData::~ReadCSVData()
{
    delete motiPos;
    delete date;

    for( int i = 0 ; i < 90000 ; i ++ )
        delete []senChannelZ[i] ;
    delete []senChannelZ;

    for( int i = 0 ; i < 90000 ; i ++ )
        delete []senChannelY[i] ;
    delete []senChannelY;

    for( int i = 0 ; i < 90000 ; i ++ )
        delete []senChannelX[i] ;
    delete []senChannelX;

    for( int i = 0 ; i < 90000 ; i ++ )
        delete []senChannelNum[i] ;
    delete []senChannelNum;

    delete pointBuffer;

    delete dynPointBuffer;
}

//该函数功能为解析文件路径，动态生成存储数组
void ReadCSVData::parseCSVFileName(QString filePath)
{
    QStringList fileSplits = filePath.split('/');
    QString sensorNumber = fileSplits[fileSplits.length()-1].split(' ')[0];
    senNum = sensorNumber.length();
    qDebug()<<"senNum="<<senNum;
    //存储每个传感器波形的激发位置，在CSV文件中表现为第H列，P列，X列...以此类推
    motiPos = new int[senNum];

    //存储每个事件触发日期，在CSV文件中表现为每行的第一列，即日期
    date = new QString[(startTime+endTime)*5000];

    //存储后三个通道中Z轴的数据，在CSV文件中表现为G列，O列，W列...以此类推
    senChannelZ = new QString*[(startTime+endTime)*(FREQUENCY+200)];
    for (int i = 0; i < ((startTime+endTime)*5000); i++){
        senChannelZ[i] = new QString[senNum];
      //  qDebug()<<"senChannel[i].size="<<senChannel[i]->size();
    }

    //存储后三个通道中Y轴的数据，在CSV文件中表现为F列，N列，V列...以此类推
    senChannelY = new QString*[(startTime+endTime)*(FREQUENCY+200)];
    for (int i = 0; i < ((startTime+endTime)*5000); i++){
        senChannelY[i] = new QString[senNum];
      //  qDebug()<<"senChannel[i].size="<<senChannel[i]->size();
    }

    //存储后三个通道中Z轴的数据，在CSV文件中表现为E列，M列，U列...以此类推
    senChannelX = new QString*[(startTime+endTime)*(FREQUENCY+200)];
    for (int i = 0; i < ((startTime+endTime)*5000); i++){
        senChannelX[i] = new QString[senNum];
      //  qDebug()<<"senChannel[i].size="<<senChannel[i]->size();
    }

    //存储每个事件触发的台站名称，在CSV文件中表现为I列，Q列，Y列...以此类推
    senChannelNum = new QString*[(startTime+endTime)*(FREQUENCY+200)];
    for (int i = 0; i < ((startTime+endTime)*5000); i++){
        senChannelNum[i] = new QString[senNum];
      //  qDebug()<<"senChannelNum[i].size="<<senChannelNum[i]->size();
    }

    //存储每个事件触发台站的X，Y，Z轴的数据
    pointBuffer = new QVector<QPointF>[27];

    //用于绘制P波到时（红线）
    pointBuffer_P = new QVector<QPointF>[10];

    //存储每个事件触发台站的X，Y，Z轴的数据,用于绘制动态波形
    dynPointBuffer = new QQueue<double>[27];

    qDebug()<<"parsing the csv file completed";
}

//说明，Dos和Windows采用回车+换行CR/LF表示下一行，
//而UNIX/Linux采用换行符LF表示下一行，
//苹果机(MAC OS系统)则采用回车符CR表示下一行
//由于项目数据的换行符为Macintosh(CR)，而且添加了'\t',所以下面注释掉的readCSVFile(QString fileName)
//因为格式差异所以不能按行读取原始CSV文件，但是在windows上用notepad++将CSV文件重新复制保存后可以按行读取
//因此，下面注释掉的代码适合于Windows格式的数据
//读取CSV文件数据
//void ReadCSVData::readCSVFile(QString fileName)
//{
//    QDir dir = QDir::current();
//    QFile file(dir.filePath(fileName));
//    //qDebug()<<"fileName="<<fileName<<'\n';
//    QList<QStringList> data;
//    QStringList item;
//    QString line ;
//    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
//        qDebug()<<"open file failed!"<<'\n';
//        return ;
//    }
//    QTextStream stream(&file);
//    while (!stream.atEnd())
//    {
//        line = stream.readLine();
//        line = line.trimmed();
//        //qDebug()<<"line = "<<line<<'\n';
//        if(line.isEmpty()){
//            qDebug()<<"CSV FILE HAS READ DONE！"<<'\n';
//            break;
//        }
//        //item = line.split(',',QString::SkipEmptyParts);                           //将读取的每一行用,分割
//        item = line.split(',');
//        date[count] = item.at(0);                         //存储每个事件的日期
//        for(int i=0;i<senNum;i++) {
//            senChannelZ[count][i] = item.at(6+i*8);       //存储每个事件后三个通道中Z轴的数据
//            senChannelY[count][i] = item.at(5+i*8);       //存储每个事件后三个通道中Y轴的数据
//            senChannelX[count][i] = item.at(4+i*8);       //存储每个事件后三个通道中X轴的数据
//            senChannelNum[count][i] = item.at(8+i*8);     //存储每个事件触发台站名称
//            motiPos[i] = item.at(7+i*8).toInt();          //存储每个传感器波形激发位置
//            //qDebug()<<"motiPos["<<i<<"] = "<<motiPos[i]<<'\n';
//        }
//        count++;
//    }
//    file.close();
//    qDebug()<<"count ="<<count<<'\n';
//    qDebug()<<"read csv file successfully!"<<'\n';
//}

//读取CSV文件数据
void ReadCSVData::readCSVFile(QString fileName)
{
    QDir dir = QDir::current();
    QFile file(dir.filePath(fileName));
    //qDebug()<<"fileName="<<fileName<<'\n';
    QStringList line;
    QStringList item;
    QString block ;
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        qDebug()<<"open file failed!";
        return ;
    }
    QTextStream stream(&file);

    while (!stream.atEnd())
    {
        block = stream.readAll();
        line = block.split('\t');
    }
    count = line.size();
    qDebug()<<"count ="<<count;
    for(int i=0;i<count-1;i++){
        item = line.at(i).split(',');
        date[i] = item.at(0);                         //存储每个事件的日期
        for(int j=0;j<senNum;j++){
            senChannelZ[i][j] = item.at(6+j*8);       //存储每个事件后三个通道中Z轴的数据
            senChannelX[i][j] = item.at(4+j*8);       //存储每个事件后三个通道中X轴的数据
            senChannelY[i][j] = item.at(5+j*8);       //存储每个事件后三个通道中Y轴的数据
            senChannelNum[i][j] = item.at(8+j*8);     //存储每个事件触发台站名称
            motiPos[j] = item.at(7+j*8).toInt();      //存储每个传感器波形激发位置
            //qDebug()<<"senChannelX[i][j]="<<senChannelX[i][j]<<'\n';
        }
        //qDebug()<<"i="<<i<<'\n';
    }
    file.close();
    qDebug()<<"read csv file successfully!";
}

void ReadCSVData::locateCSVData()
{
    int tempStation[9]={0};                                        //存储站台名称，最多有9个站台

    for(int i=0;i<senNum;i++){
        tempStation[i] = (senChannelNum[0][i]).toInt();            //获取站台名称并存储
        //qDebug()<<"tempStation[i]="<<tempStation[i]<<'\n';
    }

    for(int i=0;i<senNum;i++){
        switch(tempStation[i])                                     //判断是哪一个站台
        {
            //因为CSV文件中站名称是无序的，所以必须要定位到该站台在哪一列，从而才能够定位到该站台X，Y，Z轴以及激发位置在哪一列
            case 1:
                    //qDebug()<<"start case 1"<<'\n';
                    if(1==tempStation[0]){                         //表示读取CSV文件数据时每行的第一个站台为1站台
                        //qDebug()<<" case 1:enter paddingPointBuffer"<<'\n';
                        paddingPointBuffer(&pointBuffer[P1X],&pointBuffer[P1Y],&pointBuffer[P1Z],INDEX1);
                        paddingDynPointBuffer(&dynPointBuffer[P1X],&dynPointBuffer[P1Y],&dynPointBuffer[P1Z],INDEX1);
                        //qDebug()<<" case 1:out paddingPointBuffer"<<'\n';
                        tempMotiPos[tempStation[i]][0] = motiPos[0];            //表示tempStation[i]=x站台的激发位置，存储在tempMotiPos[x][0]中
                    }else if(1==tempStation[1]){                   //表示读取CSV文件数据时每行的第二个站台为1站台
                        paddingPointBuffer(&pointBuffer[P1X],&pointBuffer[P1Y],&pointBuffer[P1Z],INDEX2);
                        paddingDynPointBuffer(&dynPointBuffer[P1X],&dynPointBuffer[P1Y],&dynPointBuffer[P1Z],INDEX2);
                        tempMotiPos[tempStation[i]][0] = motiPos[1];               //
                    }else if(1==tempStation[2]){                   //表示读取CSV文件数据时每行的第三个站台为1站台，后面以此类推
                        paddingPointBuffer(&pointBuffer[P1X],&pointBuffer[P1Y],&pointBuffer[P1Z],INDEX3);
                        paddingDynPointBuffer(&dynPointBuffer[P1X],&dynPointBuffer[P1Y],&dynPointBuffer[P1Z],INDEX3);
                        tempMotiPos[tempStation[i]][0] = motiPos[2];
                    }else if(1==tempStation[3]){
                        paddingPointBuffer(&pointBuffer[P1X],&pointBuffer[P1Y],&pointBuffer[P1Z],INDEX4);
                        paddingDynPointBuffer(&dynPointBuffer[P1X],&dynPointBuffer[P1Y],&dynPointBuffer[P1Z],INDEX4);
                        tempMotiPos[tempStation[i]][0] = motiPos[3];
                    }else if(1==tempStation[4]){
                        paddingPointBuffer(&pointBuffer[P1X],&pointBuffer[P1Y],&pointBuffer[P1Z],INDEX5);
                        paddingDynPointBuffer(&dynPointBuffer[P1X],&dynPointBuffer[P1Y],&dynPointBuffer[P1Z],INDEX5);
                        tempMotiPos[tempStation[i]][0] = motiPos[4];
                    }else if(1==tempStation[5]){
                        paddingPointBuffer(&pointBuffer[P1X],&pointBuffer[P1Y],&pointBuffer[P1Z],INDEX6);
                        paddingDynPointBuffer(&dynPointBuffer[P1X],&dynPointBuffer[P1Y],&dynPointBuffer[P1Z],INDEX6);
                        tempMotiPos[tempStation[i]][0] = motiPos[5];
                    }else if(1==tempStation[6]){
                        paddingPointBuffer(&pointBuffer[P1X],&pointBuffer[P1Y],&pointBuffer[P1Z],INDEX7);
                        paddingDynPointBuffer(&dynPointBuffer[P1X],&dynPointBuffer[P1Y],&dynPointBuffer[P1Z],INDEX7);
                        tempMotiPos[tempStation[i]][0] = motiPos[6];
                    }else if(1==tempStation[7]){
                        paddingPointBuffer(&pointBuffer[P1X],&pointBuffer[P1Y],&pointBuffer[P1Z],INDEX8);
                        paddingDynPointBuffer(&dynPointBuffer[P1X],&dynPointBuffer[P1Y],&dynPointBuffer[P1Z],INDEX8);
                        tempMotiPos[tempStation[i]][0] = motiPos[7];
                    }else if(1==tempStation[8]){
                        paddingPointBuffer(&pointBuffer[P1X],&pointBuffer[P1Y],&pointBuffer[P1Z],INDEX9);
                        paddingDynPointBuffer(&dynPointBuffer[P1X],&dynPointBuffer[P1Y],&dynPointBuffer[P1Z],INDEX9);
                        tempMotiPos[tempStation[i]][0] = motiPos[8];
                    }else{
                        qDebug()<<"error, please check!"<<'\n';
                    }
                    //qDebug()<<"end case 1"<<'\n';
                    break;
            case 2:
                    if(2==tempStation[0]){                         //表示读取CSV文件数据时每行的第一个站台为2站台,以此类推
                        paddingPointBuffer(&pointBuffer[P2X],&pointBuffer[P2Y],&pointBuffer[P2Z],INDEX1);
                        paddingDynPointBuffer(&dynPointBuffer[P2X],&dynPointBuffer[P2Y],&dynPointBuffer[P2Z],INDEX1);
                        tempMotiPos[tempStation[i]][0] = motiPos[0];
                    }else if(2==tempStation[1]){                   //表示读取CSV文件数据时每行的第二个站台为2站台，后面以此类推
                        paddingPointBuffer(&pointBuffer[P2X],&pointBuffer[P2Y],&pointBuffer[P2Z],INDEX2);
                        paddingDynPointBuffer(&dynPointBuffer[P2X],&dynPointBuffer[P2Y],&dynPointBuffer[P2Z],INDEX2);
                        tempMotiPos[tempStation[i]][0] = motiPos[1];
                    }else if(2==tempStation[2]){
                        paddingPointBuffer(&pointBuffer[P2X],&pointBuffer[P2Y],&pointBuffer[P2Z],INDEX3);
                        paddingDynPointBuffer(&dynPointBuffer[P2X],&dynPointBuffer[P2Y],&dynPointBuffer[P2Z],INDEX3);
                        tempMotiPos[tempStation[i]][0] = motiPos[2];
                    }else if(2==tempStation[3]){
                        paddingPointBuffer(&pointBuffer[P2X],&pointBuffer[P2Y],&pointBuffer[P2Z],INDEX4);
                        paddingDynPointBuffer(&dynPointBuffer[P2X],&dynPointBuffer[P2Y],&dynPointBuffer[P2Z],INDEX4);
                        tempMotiPos[tempStation[i]][0] = motiPos[3];
                    }else if(2==tempStation[4]){
                        paddingPointBuffer(&pointBuffer[P2X],&pointBuffer[P2Y],&pointBuffer[P2Z],INDEX5);
                        paddingDynPointBuffer(&dynPointBuffer[P2X],&dynPointBuffer[P2Y],&dynPointBuffer[P2Z],INDEX5);
                        tempMotiPos[tempStation[i]][0] = motiPos[4];
                    }else if(2==tempStation[5]){
                        paddingPointBuffer(&pointBuffer[P2X],&pointBuffer[P2Y],&pointBuffer[P2Z],INDEX6);
                        paddingDynPointBuffer(&dynPointBuffer[P2X],&dynPointBuffer[P2Y],&dynPointBuffer[P2Z],INDEX6);
                        tempMotiPos[tempStation[i]][0] = motiPos[5];
                    }else if(2==tempStation[6]){
                        paddingPointBuffer(&pointBuffer[P2X],&pointBuffer[P2Y],&pointBuffer[P2Z],INDEX7);
                        paddingDynPointBuffer(&dynPointBuffer[P2X],&dynPointBuffer[P2Y],&dynPointBuffer[P2Z],INDEX7);
                        tempMotiPos[tempStation[i]][0] = motiPos[6];
                    }else if(2==tempStation[7]){
                        paddingPointBuffer(&pointBuffer[P2X],&pointBuffer[P2Y],&pointBuffer[P2Z],INDEX8);
                        paddingDynPointBuffer(&dynPointBuffer[P2X],&dynPointBuffer[P2Y],&dynPointBuffer[P2Z],INDEX8);
                        tempMotiPos[tempStation[i]][0] = motiPos[7];
                    }else if(2==tempStation[8]){
                        paddingPointBuffer(&pointBuffer[P2X],&pointBuffer[P2Y],&pointBuffer[P2Z],INDEX9);
                        paddingDynPointBuffer(&dynPointBuffer[P2X],&dynPointBuffer[P2Y],&dynPointBuffer[P2Z],INDEX9);
                        tempMotiPos[tempStation[i]][0] = motiPos[8];
                    }else{
                        qDebug()<<"error, please check!"<<'\n';
                    }break;
            case 3:
                    if(3==tempStation[0]){                         //表示读取CSV文件数据时每行的第一个站台为3站台,以此类推
                        paddingPointBuffer(&pointBuffer[P3X],&pointBuffer[P3Y],&pointBuffer[P3Z],INDEX1);
                        paddingDynPointBuffer(&dynPointBuffer[P3X],&dynPointBuffer[P3Y],&dynPointBuffer[P3Z],INDEX1);
                        tempMotiPos[tempStation[i]][0] = motiPos[0];
                    }else if(3==tempStation[1]){                   //表示读取CSV文件数据时每行的第二个站台为3站台，后面以此类推
                        paddingPointBuffer(&pointBuffer[P3X],&pointBuffer[P3Y],&pointBuffer[P3Z],INDEX2);
                        paddingDynPointBuffer(&dynPointBuffer[P3X],&dynPointBuffer[P3Y],&dynPointBuffer[P3Z],INDEX2);
                        tempMotiPos[tempStation[i]][0] = motiPos[1];
                    }else if(3==tempStation[2]){
                        paddingPointBuffer(&pointBuffer[P3X],&pointBuffer[P3Y],&pointBuffer[P3Z],INDEX3);
                        paddingDynPointBuffer(&dynPointBuffer[P3X],&dynPointBuffer[P3Y],&dynPointBuffer[P3Z],INDEX3);
                        tempMotiPos[tempStation[i]][0] = motiPos[2];
                    }else if(3==tempStation[3]){
                        paddingPointBuffer(&pointBuffer[P3X],&pointBuffer[P3Y],&pointBuffer[P3Z],INDEX4);
                        paddingDynPointBuffer(&dynPointBuffer[P3X],&dynPointBuffer[P3Y],&dynPointBuffer[P3Z],INDEX4);
                        tempMotiPos[tempStation[i]][0] = motiPos[3];
                    }else if(3==tempStation[4]){
                        paddingPointBuffer(&pointBuffer[P3X],&pointBuffer[P3Y],&pointBuffer[P3Z],INDEX5);
                        paddingDynPointBuffer(&dynPointBuffer[P3X],&dynPointBuffer[P3Y],&dynPointBuffer[P3Z],INDEX5);
                        tempMotiPos[tempStation[i]][0] = motiPos[4];
                    }else if(3==tempStation[5]){
                        paddingPointBuffer(&pointBuffer[P3X],&pointBuffer[P3Y],&pointBuffer[P3Z],INDEX6);
                        paddingDynPointBuffer(&dynPointBuffer[P3X],&dynPointBuffer[P3Y],&dynPointBuffer[P3Z],INDEX6);
                        tempMotiPos[tempStation[i]][0] = motiPos[5];
                    }else if(3==tempStation[6]){
                        paddingPointBuffer(&pointBuffer[P3X],&pointBuffer[P3Y],&pointBuffer[P3Z],INDEX7);
                        paddingDynPointBuffer(&dynPointBuffer[P3X],&dynPointBuffer[P3Y],&dynPointBuffer[P3Z],INDEX7);
                        tempMotiPos[tempStation[i]][0] = motiPos[6];
                    }else if(3==tempStation[7]){
                        paddingPointBuffer(&pointBuffer[P3X],&pointBuffer[P3Y],&pointBuffer[P3Z],INDEX8);
                        paddingDynPointBuffer(&dynPointBuffer[P3X],&dynPointBuffer[P3Y],&dynPointBuffer[P3Z],INDEX8);
                        tempMotiPos[tempStation[i]][0] = motiPos[7];
                    }else if(3==tempStation[8]){
                        paddingPointBuffer(&pointBuffer[P3X],&pointBuffer[P3Y],&pointBuffer[P3Z],INDEX9);
                        paddingDynPointBuffer(&dynPointBuffer[P3X],&dynPointBuffer[P3Y],&dynPointBuffer[P3Z],INDEX9);
                        tempMotiPos[tempStation[i]][0] = motiPos[8];
                    }else{
                        qDebug()<<"error, please check!"<<'\n';
                    }break;
            case 4:
                    if(4==tempStation[0]){                         //表示读取CSV文件数据时每行的第一个站台为4站台,以此类推
                        paddingPointBuffer(&pointBuffer[P4X],&pointBuffer[P4Y],&pointBuffer[P4Z],INDEX1);
                        paddingDynPointBuffer(&dynPointBuffer[P4X],&dynPointBuffer[P4Y],&dynPointBuffer[P4Z],INDEX1);
                        tempMotiPos[tempStation[i]][0] = motiPos[0];
                    }else if(4==tempStation[1]){                   //表示读取CSV文件数据时每行的第二个站台为4站台，后面以此类推
                        paddingPointBuffer(&pointBuffer[P4X],&pointBuffer[P4Y],&pointBuffer[P4Z],INDEX2);
                        paddingDynPointBuffer(&dynPointBuffer[P4X],&dynPointBuffer[P4Y],&dynPointBuffer[P4Z],INDEX2);
                        tempMotiPos[tempStation[i]][0] = motiPos[1];
                    }else if(4==tempStation[2]){
                        paddingPointBuffer(&pointBuffer[P4X],&pointBuffer[P4Y],&pointBuffer[P4Z],INDEX3);
                        paddingDynPointBuffer(&dynPointBuffer[P4X],&dynPointBuffer[P4Y],&dynPointBuffer[P4Z],INDEX3);
                        tempMotiPos[tempStation[i]][0] = motiPos[2];
                    }else if(4==tempStation[3]){
                        paddingPointBuffer(&pointBuffer[P4X],&pointBuffer[P4Y],&pointBuffer[P4Z],INDEX4);
                        paddingDynPointBuffer(&dynPointBuffer[P4X],&dynPointBuffer[P4Y],&dynPointBuffer[P4Z],INDEX4);
                        tempMotiPos[tempStation[i]][0] = motiPos[3];
                    }else if(4==tempStation[4]){
                        paddingPointBuffer(&pointBuffer[P4X],&pointBuffer[P4Y],&pointBuffer[P4Z],INDEX5);
                        paddingDynPointBuffer(&dynPointBuffer[P4X],&dynPointBuffer[P4Y],&dynPointBuffer[P4Z],INDEX5);
                        tempMotiPos[tempStation[i]][0] = motiPos[4];
                    }else if(4==tempStation[5]){
                        paddingPointBuffer(&pointBuffer[P4X],&pointBuffer[P4Y],&pointBuffer[P4Z],INDEX6);
                        paddingDynPointBuffer(&dynPointBuffer[P4X],&dynPointBuffer[P4Y],&dynPointBuffer[P4Z],INDEX6);
                        tempMotiPos[tempStation[i]][0] = motiPos[5];
                    }else if(4==tempStation[6]){
                        paddingPointBuffer(&pointBuffer[P4X],&pointBuffer[P4Y],&pointBuffer[P4Z],INDEX7);
                        paddingDynPointBuffer(&dynPointBuffer[P4X],&dynPointBuffer[P4Y],&dynPointBuffer[P4Z],INDEX7);
                        tempMotiPos[tempStation[i]][0] = motiPos[6];
                    }else if(4==tempStation[7]){
                        paddingPointBuffer(&pointBuffer[P4X],&pointBuffer[P4Y],&pointBuffer[P4Z],INDEX8);
                        paddingDynPointBuffer(&dynPointBuffer[P4X],&dynPointBuffer[P4Y],&dynPointBuffer[P4Z],INDEX8);
                        tempMotiPos[tempStation[i]][0] = motiPos[7];
                    }else if(4==tempStation[8]){
                        paddingPointBuffer(&pointBuffer[P4X],&pointBuffer[P4Y],&pointBuffer[P4Z],INDEX9);
                        paddingDynPointBuffer(&dynPointBuffer[P4X],&dynPointBuffer[P4Y],&dynPointBuffer[P4Z],INDEX9);
                        tempMotiPos[tempStation[i]][0] = motiPos[8];
                    }else{
                        qDebug()<<"error, please check!"<<'\n';
                    }break;
            case 5:
                    if(5==tempStation[0]){                         //表示读取CSV文件数据时每行的第一个站台为5站台,以此类推
                        paddingPointBuffer(&pointBuffer[P5X],&pointBuffer[P5Y],&pointBuffer[P5Z],INDEX1);
                        paddingDynPointBuffer(&dynPointBuffer[P5X],&dynPointBuffer[P5Y],&dynPointBuffer[P5Z],INDEX1);
                        tempMotiPos[tempStation[i]][0] = motiPos[0];
                    }else if(5==tempStation[1]){                   //表示读取CSV文件数据时每行的第二个站台为5站台，后面以此类推
                        paddingPointBuffer(&pointBuffer[P5X],&pointBuffer[P5Y],&pointBuffer[P5Z],INDEX2);
                        paddingDynPointBuffer(&dynPointBuffer[P5X],&dynPointBuffer[P5Y],&dynPointBuffer[P5Z],INDEX2);
                        tempMotiPos[tempStation[i]][0] = motiPos[1];
                    }else if(5==tempStation[2]){
                        paddingPointBuffer(&pointBuffer[P5X],&pointBuffer[P5Y],&pointBuffer[P5Z],INDEX3);
                        paddingDynPointBuffer(&dynPointBuffer[P5X],&dynPointBuffer[P5Y],&dynPointBuffer[P5Z],INDEX3);
                        tempMotiPos[tempStation[i]][0] = motiPos[2];
                    }else if(5==tempStation[3]){
                        paddingPointBuffer(&pointBuffer[P5X],&pointBuffer[P5Y],&pointBuffer[P5Z],INDEX4);
                        paddingDynPointBuffer(&dynPointBuffer[P5X],&dynPointBuffer[P5Y],&dynPointBuffer[P5Z],INDEX4);
                        tempMotiPos[tempStation[i]][0] = motiPos[3];
                    }else if(5==tempStation[4]){
                        paddingPointBuffer(&pointBuffer[P5X],&pointBuffer[P5Y],&pointBuffer[P5Z],INDEX5);
                        paddingDynPointBuffer(&dynPointBuffer[P5X],&dynPointBuffer[P5Y],&dynPointBuffer[P5Z],INDEX5);
                        tempMotiPos[tempStation[i]][0] = motiPos[4];
                    }else if(5==tempStation[5]){
                        paddingPointBuffer(&pointBuffer[P5X],&pointBuffer[P5Y],&pointBuffer[P5Z],INDEX6);
                        paddingDynPointBuffer(&dynPointBuffer[P5X],&dynPointBuffer[P5Y],&dynPointBuffer[P5Z],INDEX6);
                        tempMotiPos[tempStation[i]][0] = motiPos[5];
                    }else if(5==tempStation[6]){
                        paddingPointBuffer(&pointBuffer[P5X],&pointBuffer[P5Y],&pointBuffer[P5Z],INDEX7);
                        paddingDynPointBuffer(&dynPointBuffer[P5X],&dynPointBuffer[P5Y],&dynPointBuffer[P5Z],INDEX7);
                        tempMotiPos[tempStation[i]][0] = motiPos[6];
                    }else if(5==tempStation[7]){
                        paddingPointBuffer(&pointBuffer[P5X],&pointBuffer[P5Y],&pointBuffer[P5Z],INDEX8);
                        paddingDynPointBuffer(&dynPointBuffer[P5X],&dynPointBuffer[P5Y],&dynPointBuffer[P5Z],INDEX8);
                        tempMotiPos[tempStation[i]][0] = motiPos[7];
                    }else if(5==tempStation[8]){
                        paddingPointBuffer(&pointBuffer[P5X],&pointBuffer[P5Y],&pointBuffer[P5Z],INDEX9);
                        paddingDynPointBuffer(&dynPointBuffer[P5X],&dynPointBuffer[P5Y],&dynPointBuffer[P5Z],INDEX9);
                        tempMotiPos[tempStation[i]][0] = motiPos[8];
                    }else{
                        qDebug()<<"error, please check!"<<'\n';
                    }break;
            case 6:
                    if(6==tempStation[0]){                         //表示读取CSV文件数据时每行的第一个站台为6站台,以此类推
                        paddingPointBuffer(&pointBuffer[P6X],&pointBuffer[P6Y],&pointBuffer[P6Z],INDEX1);
                        paddingDynPointBuffer(&dynPointBuffer[P6X],&dynPointBuffer[P6Y],&dynPointBuffer[P6Z],INDEX1);
                        tempMotiPos[tempStation[i]][0] = motiPos[0];
                    }else if(6==tempStation[1]){                   //表示读取CSV文件数据时每行的第二个站台为6站台，后面以此类推
                        paddingPointBuffer(&pointBuffer[P6X],&pointBuffer[P6Y],&pointBuffer[P6Z],INDEX2);
                        paddingDynPointBuffer(&dynPointBuffer[P6X],&dynPointBuffer[P6Y],&dynPointBuffer[P6Z],INDEX2);
                        tempMotiPos[tempStation[i]][0] = motiPos[1];
                    }else if(6==tempStation[2]){
                        paddingPointBuffer(&pointBuffer[P6X],&pointBuffer[P6Y],&pointBuffer[P6Z],INDEX3);
                        paddingDynPointBuffer(&dynPointBuffer[P6X],&dynPointBuffer[P6Y],&dynPointBuffer[P6Z],INDEX3);
                        tempMotiPos[tempStation[i]][0] = motiPos[2];
                    }else if(6==tempStation[3]){
                        paddingPointBuffer(&pointBuffer[P6X],&pointBuffer[P6Y],&pointBuffer[P6Z],INDEX4);
                        paddingDynPointBuffer(&dynPointBuffer[P6X],&dynPointBuffer[P6Y],&dynPointBuffer[P6Z],INDEX4);
                        tempMotiPos[tempStation[i]][0] = motiPos[3];
                    }else if(6==tempStation[4]){
                        paddingPointBuffer(&pointBuffer[P6X],&pointBuffer[P6Y],&pointBuffer[P6Z],INDEX5);
                        paddingDynPointBuffer(&dynPointBuffer[P6X],&dynPointBuffer[P6Y],&dynPointBuffer[P6Z],INDEX5);
                        tempMotiPos[tempStation[i]][0] = motiPos[4];
                    }else if(6==tempStation[5]){
                        paddingPointBuffer(&pointBuffer[P6X],&pointBuffer[P6Y],&pointBuffer[P6Z],INDEX6);
                        paddingDynPointBuffer(&dynPointBuffer[P6X],&dynPointBuffer[P6Y],&dynPointBuffer[P6Z],INDEX6);
                        tempMotiPos[tempStation[i]][0] = motiPos[5];
                    }else if(6==tempStation[6]){
                        paddingPointBuffer(&pointBuffer[P6X],&pointBuffer[P6Y],&pointBuffer[P6Z],INDEX7);
                        paddingDynPointBuffer(&dynPointBuffer[P6X],&dynPointBuffer[P6Y],&dynPointBuffer[P6Z],INDEX7);
                        tempMotiPos[tempStation[i]][0] = motiPos[6];
                    }else if(6==tempStation[7]){
                        paddingPointBuffer(&pointBuffer[P6X],&pointBuffer[P6Y],&pointBuffer[P6Z],INDEX8);
                        paddingDynPointBuffer(&dynPointBuffer[P6X],&dynPointBuffer[P6Y],&dynPointBuffer[P6Z],INDEX8);
                        tempMotiPos[tempStation[i]][0] = motiPos[7];
                    }else if(6==tempStation[8]){
                        paddingPointBuffer(&pointBuffer[P6X],&pointBuffer[P6Y],&pointBuffer[P6Z],INDEX9);
                        paddingDynPointBuffer(&dynPointBuffer[P6X],&dynPointBuffer[P6Y],&dynPointBuffer[P6Z],INDEX9);
                        tempMotiPos[tempStation[i]][0] = motiPos[8];
                    }else{
                        qDebug()<<"error, please check!"<<'\n';
                    }break;
            case 7:
                if(7==tempStation[0]){                         //表示读取CSV文件数据时每行的第一个站台为7站台,以此类推
                    paddingPointBuffer(&pointBuffer[P7X],&pointBuffer[P7Y],&pointBuffer[P7Z],INDEX1);
                    paddingDynPointBuffer(&dynPointBuffer[P7X],&dynPointBuffer[P7Y],&dynPointBuffer[P7Z],INDEX1);
                    tempMotiPos[tempStation[i]][0] = motiPos[0];
                }else if(7==tempStation[1]){                   //表示读取CSV文件数据时每行的第二个站台为7站台，后面以此类推
                    paddingPointBuffer(&pointBuffer[P7X],&pointBuffer[P7Y],&pointBuffer[P7Z],INDEX2);
                    paddingDynPointBuffer(&dynPointBuffer[P7X],&dynPointBuffer[P7Y],&dynPointBuffer[P7Z],INDEX2);
                    tempMotiPos[tempStation[i]][0] = motiPos[1];
                }else if(7==tempStation[2]){
                    paddingPointBuffer(&pointBuffer[P7X],&pointBuffer[P7Y],&pointBuffer[P7Z],INDEX3);
                    paddingDynPointBuffer(&dynPointBuffer[P7X],&dynPointBuffer[P7Y],&dynPointBuffer[P7Z],INDEX3);
                    tempMotiPos[tempStation[i]][0] = motiPos[2];
                }else if(7==tempStation[3]){
                    paddingPointBuffer(&pointBuffer[P7X],&pointBuffer[P7Y],&pointBuffer[P7Z],INDEX4);
                    paddingDynPointBuffer(&dynPointBuffer[P7X],&dynPointBuffer[P7Y],&dynPointBuffer[P7Z],INDEX4);
                    tempMotiPos[tempStation[i]][0] = motiPos[3];
                }else if(7==tempStation[4]){
                    paddingPointBuffer(&pointBuffer[P7X],&pointBuffer[P7Y],&pointBuffer[P7Z],INDEX5);
                    paddingDynPointBuffer(&dynPointBuffer[P7X],&dynPointBuffer[P7Y],&dynPointBuffer[P7Z],INDEX5);
                    tempMotiPos[tempStation[i]][0] = motiPos[4];
                }else if(7==tempStation[5]){
                    paddingPointBuffer(&pointBuffer[P7X],&pointBuffer[P7Y],&pointBuffer[P7Z],INDEX6);
                    paddingDynPointBuffer(&dynPointBuffer[P7X],&dynPointBuffer[P7Y],&dynPointBuffer[P7Z],INDEX6);
                    tempMotiPos[tempStation[i]][0] = motiPos[5];
                }else if(7==tempStation[6]){
                    paddingPointBuffer(&pointBuffer[P7X],&pointBuffer[P7Y],&pointBuffer[P7Z],INDEX7);
                    paddingDynPointBuffer(&dynPointBuffer[P7X],&dynPointBuffer[P7Y],&dynPointBuffer[P7Z],INDEX7);
                    tempMotiPos[tempStation[i]][0] = motiPos[6];
                }else if(7==tempStation[7]){
                    paddingPointBuffer(&pointBuffer[P7X],&pointBuffer[P7Y],&pointBuffer[P7Z],INDEX8);
                    paddingDynPointBuffer(&dynPointBuffer[P7X],&dynPointBuffer[P7Y],&dynPointBuffer[P7Z],INDEX8);
                    tempMotiPos[tempStation[i]][0] = motiPos[7];
                }else if(7==tempStation[8]){
                    paddingPointBuffer(&pointBuffer[P7X],&pointBuffer[P7Y],&pointBuffer[P7Z],INDEX9);
                    paddingDynPointBuffer(&dynPointBuffer[P7X],&dynPointBuffer[P7Y],&dynPointBuffer[P7Z],INDEX9);
                    tempMotiPos[tempStation[i]][0] = motiPos[8];
                }else{
                    qDebug()<<"error, please check!"<<'\n';
                }break;
            case 8:
                if(8==tempStation[0]){                         //表示读取CSV文件数据时每行的第一个站台为8站台,以此类推
                    paddingPointBuffer(&pointBuffer[P8X],&pointBuffer[P8Y],&pointBuffer[P8Z],INDEX1);
                    paddingDynPointBuffer(&dynPointBuffer[P8X],&dynPointBuffer[P8Y],&dynPointBuffer[P8Z],INDEX1);
                    tempMotiPos[tempStation[i]][0] = motiPos[0];
                }else if(8==tempStation[1]){                   //表示读取CSV文件数据时每行的第二个站台为8站台，后面以此类推
                    paddingPointBuffer(&pointBuffer[P8X],&pointBuffer[P8Y],&pointBuffer[P8Z],INDEX2);
                    paddingDynPointBuffer(&dynPointBuffer[P8X],&dynPointBuffer[P8Y],&dynPointBuffer[P8Z],INDEX2);
                    tempMotiPos[tempStation[i]][0] = motiPos[1];
                }else if(8==tempStation[2]){
                    paddingPointBuffer(&pointBuffer[P8X],&pointBuffer[P8Y],&pointBuffer[P8Z],INDEX3);
                    paddingDynPointBuffer(&dynPointBuffer[P8X],&dynPointBuffer[P8Y],&dynPointBuffer[P8Z],INDEX3);
                    tempMotiPos[tempStation[i]][0] = motiPos[2];
                }else if(8==tempStation[3]){
                    paddingPointBuffer(&pointBuffer[P8X],&pointBuffer[P8Y],&pointBuffer[P8Z],INDEX4);
                    paddingDynPointBuffer(&dynPointBuffer[P8X],&dynPointBuffer[P8Y],&dynPointBuffer[P8Z],INDEX4);
                    tempMotiPos[tempStation[i]][0] = motiPos[3];
                }else if(8==tempStation[4]){
                    paddingPointBuffer(&pointBuffer[P8X],&pointBuffer[P8Y],&pointBuffer[P8Z],INDEX5);
                    paddingDynPointBuffer(&dynPointBuffer[P8X],&dynPointBuffer[P8Y],&dynPointBuffer[P8Z],INDEX5);
                    tempMotiPos[tempStation[i]][0] = motiPos[4];
                }else if(8==tempStation[5]){
                    paddingPointBuffer(&pointBuffer[P8X],&pointBuffer[P8Y],&pointBuffer[P8Z],INDEX6);
                    paddingDynPointBuffer(&dynPointBuffer[P8X],&dynPointBuffer[P8Y],&dynPointBuffer[P8Z],INDEX6);
                    tempMotiPos[tempStation[i]][0] = motiPos[5];
                }else if(8==tempStation[6]){
                    paddingPointBuffer(&pointBuffer[P8X],&pointBuffer[P8Y],&pointBuffer[P8Z],INDEX7);
                    paddingDynPointBuffer(&dynPointBuffer[P8X],&dynPointBuffer[P8Y],&dynPointBuffer[P8Z],INDEX7);
                    tempMotiPos[tempStation[i]][0] = motiPos[6];
                }else if(8==tempStation[7]){
                    paddingPointBuffer(&pointBuffer[P8X],&pointBuffer[P8Y],&pointBuffer[P8Z],INDEX8);
                    paddingDynPointBuffer(&dynPointBuffer[P8X],&dynPointBuffer[P8Y],&dynPointBuffer[P8Z],INDEX8);
                    tempMotiPos[tempStation[i]][0] = motiPos[7];
                }else if(8==tempStation[8]){
                    paddingPointBuffer(&pointBuffer[P8X],&pointBuffer[P8Y],&pointBuffer[P8Z],INDEX9);
                    paddingDynPointBuffer(&dynPointBuffer[P8X],&dynPointBuffer[P8Y],&dynPointBuffer[P8Z],INDEX9);
                    tempMotiPos[tempStation[i]][0] = motiPos[8];
                }else{
                    qDebug()<<"error, please check!"<<'\n';
                }break;
            case 9:
                if(9==tempStation[0]){                         //表示读取CSV文件数据时每行的第一个站台为6站台,以此类推
                    paddingPointBuffer(&pointBuffer[P9X],&pointBuffer[P9Y],&pointBuffer[P9Z],INDEX1);
                    paddingDynPointBuffer(&dynPointBuffer[P9X],&dynPointBuffer[P9Y],&dynPointBuffer[P9Z],INDEX1);
                    tempMotiPos[tempStation[i]][0] = motiPos[0];
                }else if(9==tempStation[1]){                   //表示读取CSV文件数据时每行的第二个站台为6站台，后面以此类推
                    paddingPointBuffer(&pointBuffer[P9X],&pointBuffer[P9Y],&pointBuffer[P9Z],INDEX2);
                    paddingDynPointBuffer(&dynPointBuffer[P9X],&dynPointBuffer[P9Y],&dynPointBuffer[P9Z],INDEX2);
                    tempMotiPos[tempStation[i]][0] = motiPos[1];
                }else if(9==tempStation[2]){
                    paddingPointBuffer(&pointBuffer[P9X],&pointBuffer[P9Y],&pointBuffer[P9Z],INDEX3);
                    paddingDynPointBuffer(&dynPointBuffer[P9X],&dynPointBuffer[P9Y],&dynPointBuffer[P9Z],INDEX3);
                    tempMotiPos[tempStation[i]][0] = motiPos[2];
                }else if(9==tempStation[3]){
                    paddingPointBuffer(&pointBuffer[P9X],&pointBuffer[P9Y],&pointBuffer[P9Z],INDEX4);
                    paddingDynPointBuffer(&dynPointBuffer[P9X],&dynPointBuffer[P9Y],&dynPointBuffer[P9Z],INDEX4);
                    tempMotiPos[tempStation[i]][0] = motiPos[3];
                }else if(9==tempStation[4]){
                    paddingPointBuffer(&pointBuffer[P9X],&pointBuffer[P9Y],&pointBuffer[P9Z],INDEX5);
                    paddingDynPointBuffer(&dynPointBuffer[P9X],&dynPointBuffer[P9Y],&dynPointBuffer[P9Z],INDEX5);
                    tempMotiPos[tempStation[i]][0] = motiPos[4];
                }else if(9==tempStation[5]){
                    paddingPointBuffer(&pointBuffer[P9X],&pointBuffer[P9Y],&pointBuffer[P9Z],INDEX6);
                    paddingDynPointBuffer(&dynPointBuffer[P9X],&dynPointBuffer[P9Y],&dynPointBuffer[P9Z],INDEX6);
                    tempMotiPos[tempStation[i]][0] = motiPos[5];
                }else if(9==tempStation[6]){
                    paddingPointBuffer(&pointBuffer[P9X],&pointBuffer[P9Y],&pointBuffer[P9Z],INDEX7);
                    paddingDynPointBuffer(&dynPointBuffer[P9X],&dynPointBuffer[P9Y],&dynPointBuffer[P9Z],INDEX7);
                    tempMotiPos[tempStation[i]][0] = motiPos[6];
                }else if(9==tempStation[7]){
                    paddingPointBuffer(&pointBuffer[P9X],&pointBuffer[P9Y],&pointBuffer[P9Z],INDEX8);
                    paddingDynPointBuffer(&dynPointBuffer[P9X],&dynPointBuffer[P9Y],&dynPointBuffer[P9Z],INDEX8);
                    tempMotiPos[tempStation[i]][0] = motiPos[7];
                }else if(9==tempStation[8]){
                    paddingPointBuffer(&pointBuffer[P9X],&pointBuffer[P9Y],&pointBuffer[P9Z],INDEX9);
                    paddingDynPointBuffer(&dynPointBuffer[P9X],&dynPointBuffer[P9Y],&dynPointBuffer[P9Z],INDEX9);
                    tempMotiPos[tempStation[i]][0] = motiPos[8];
                }else{
                    qDebug()<<"error, please check!"<<'\n';
                }break;
            default:
                  qDebug()<<"current station"<<tempStation[i]<<'\n';
        }
    }
    for(int j=0;j<10;j++){
        if(tempMotiPos[j][0]!=0){
            qDebug()<<"this station is"<<j<<"and its motivated position is "<<tempMotiPos[j][0];  //打印测试站台及其激发位置
            //因为P波到时是一条垂线，所以只需要添加横坐标相同的几个点就好
            pointBuffer_P[j].append(QPointF(tempMotiPos[j][0],0));
            pointBuffer_P[j].append(QPointF(tempMotiPos[j][0],50000));
            pointBuffer_P[j].append(QPointF(tempMotiPos[j][0],-50000));
        }
    }
    qDebug()<<"locateCSVData successfully!";
}

//入参传入QVector<QPointF>的实例化对象
void ReadCSVData::paddingPointBuffer(QVector<QPointF> *pointBufferX,QVector<QPointF> *pointBufferY,QVector<QPointF> *pointBufferZ,int index)
{
    int listIndex = 0; 	                                     //可以理解为横坐标，因为横坐标就是每个事件（也就是CSV文件中的一行数据）
    double parseDataX;                                        //存储CSV文件中字符值转换为浮点型后的值
    double parseDataY;
    double parseDataZ;
    for(int j=0;j<count;j++){
        //qDebug()<<"enter internal paddingPointBuffer!"<<'\n';
        parseDataX = senChannelX[j][index].toDouble();        //QString转为float型
        //qDebug()<<"senChannelX[j][index]="<<senChannelX[j][index]<<'\n';
        //qDebug()<<"parseDataX="<<parseDataX<<'\n';
        parseDataY = senChannelY[j][index].toDouble();
        //qDebug()<<"senChannelY[j][index]="<<senChannelY[j][index]<<'\n';
        //qDebug()<<"parseDataY="<<parseDataY<<'\n';
        parseDataZ = senChannelZ[j][index].toDouble();
        //qDebug()<<"senChannelZ[j][index]="<<senChannelZ[j][index]<<'\n';
        //qDebug()<<"parseDataZ="<<parseDataZ<<'\n';
        //qDebug()<<" between paddingPointBuffer !"<<'\n';
        pointBufferX->append(QPointF(listIndex, parseDataX)); //点坐标集合
        pointBufferY->append(QPointF(listIndex, parseDataY));
        pointBufferZ->append(QPointF(listIndex, parseDataZ));
        listIndex++;
        //qDebug()<<"leave internal paddingPointBuffer"<<'\n';
    }
    qDebug()<<"paddingPointBuffer successfully!";
}

//入参传入QQueue<double>的实例化对象
void ReadCSVData::paddingDynPointBuffer(QQueue<double> *pointBufferX,QQueue<double> *pointBufferY,QQueue<double> *pointBufferZ,int index)
{
    double parseDataX;                                        //存储CSV文件中字符值转换为浮点型后的值
    double parseDataY;
    double parseDataZ;
    for(int j=0;j<count;j++){
        //动态波形不需要显示全部数据，因此只存储部分数据
        //例如每100行CSV数据中，只存储中间值，即第50行数据
        //索引标号表示为0...99中，只保留索引为49的一行数据
        //因此90000行数据只会存储900行数据
        if(j%49 == 0 && j%2 !=0){
            parseDataX = senChannelX[j][index].toDouble();        //QString转为Double型
            parseDataY = senChannelY[j][index].toDouble();
            parseDataZ = senChannelZ[j][index].toDouble();

            pointBufferX->enqueue(parseDataX);
            pointBufferY->enqueue(parseDataY);
            pointBufferZ->enqueue(parseDataZ);
        }
    }
    qDebug()<<"paddingDynPointBuffer successfully!";
}
